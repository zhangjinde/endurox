/* 
** ATMI API functions
**
** @file atmi.c
** 
** -----------------------------------------------------------------------------
** Enduro/X Middleware Platform for Distributed Transaction Processing
** Copyright (C) 2015, Mavimax, Ltd. All Rights Reserved.
** This software is released under one of the following licenses:
** GPL or Mavimax's license for commercial use.
** -----------------------------------------------------------------------------
** GPL license:
** 
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
** PARTICULAR PURPOSE. See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 59 Temple
** Place, Suite 330, Boston, MA 02111-1307 USA
**
** -----------------------------------------------------------------------------
** A commercial use license is available from Mavimax, Ltd
** contact@mavimax.com
** -----------------------------------------------------------------------------
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <atmi.h>

#include <ndrstandard.h>
#include <atmi_tls.h>
#include <atmi_int.h>
#include <ndebug.h>
#include <Exfields.h>
#include <xa_cmn.h>
#include <tperror.h>
#include <atmi_tls.h>
#include <ubf.h>
/*---------------------------Externs------------------------------------*/
/*---------------------------Macros-------------------------------------*/
#define API_ENTRY {ndrx_TPunset_error(); \
    ATMI_TLS_ENTRY;\
    if (!G_atmi_tls->G_atmi_is_init) { \
        /* this means this is dirty client call, do the init */\
        NDRX_DBG_INIT(("ATMI", ""));\
        entry_status=tpinit(NULL);\
    }\
}\
/*---------------------------Enums--------------------------------------*/
/*---------------------------Typedefs-----------------------------------*/
/*---------------------------Globals------------------------------------*/
/*---------------------------Statics------------------------------------*/
/*---------------------------Prototypes---------------------------------*/

/**
 * API entry for tpacall
 * @param svc
 * @param data
 * @param len
 * @param flags
 * @return
 */
expublic int tpacall (char *svc, char *data, long len, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    /*flags|=TPNOREPLY;  force that we do not wait for answer! - not needed here really!
     causes problems with serice async replies!, See doc for tpacall! */
            
    ret=ndrx_tpacall(svc, data, len, flags, NULL, EXFAIL, 0, NULL); /* no reply queue */
    
out:
    return ret;
}

/**
 * Extended version of tpacall, allow extradata + event posting.
 * @param svc
 * @param data
 * @param len
 * @param flags
 * @return
 */
expublic int tpacallex (char *svc, char *data, 
        long len, long flags, char *extradata, int dest_node, int ex_flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    /*flags|=TPNOREPLY;  force that we do not wait for answer! - not needed here really!
     causes problems with serice async replies!, See doc for tpacall! */
            
    ret=ndrx_tpacall(svc, data, len, flags, extradata, dest_node, ex_flags, NULL); /* no reply queue */
    
out:
    return ret;
}

/**
 * API entry for tpalloc
 * @param type
 * @param subtype
 * @param len
 * @return
 */
expublic char * tpalloc (char *type, char *subtype, long len)
{
    char *ret=NULL;
    /* int entry_status=EXSUCCEED; */
    
/* Allow to skip initalization - this for for clt init (using tpalloc for buffer request)
    API_ENTRY;
    if (SUCCEED!=entry_status)
    {
        ret=NULL;
        goto out;
    }
 */   
    ret=ndrx_tpalloc(NULL, type, subtype, len);

out:
    return ret;
}

/**
 * API entry for tprealloc
 * @param type
 * @param subtype
 * @param len
 * @return
 */
expublic char * tprealloc (char *buf, long len)
{
    char * ret=NULL;
    int entry_status=EXSUCCEED;
    
    API_ENTRY;
    if (EXSUCCEED!=entry_status)
    {
        ret=NULL;
        goto out;
    }
    
    ret=ndrx_tprealloc(buf, len);
    
out:
    return ret;
}

/**
 * API version of tpcall
 * Extended with new flag: TPTRANSUSPEND - suspend global TX in progress.
 *          Usable only in sync mode.
 * @param svc
 * @param idata
 * @param ilen
 * @param odata
 * @param olen
 * @param flags
 * @return SUCCEED/FAIL
 */
expublic int tpcall (char *svc, char *idata, long ilen,
                char **odata, long *olen, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    /* Check some other parameters */
    if (olen==NULL)
    {
        ndrx_TPset_error_msg(TPEINVAL, "olen cannot be null");
        ret=EXFAIL;
        goto out;
    }
    
    /* Check some other parameters */
    if (odata==NULL)
    {
        ndrx_TPset_error_msg(TPEINVAL, "odata cannot be null");
        ret=EXFAIL;
        goto out;
    }
    
    if (flags & TPNOREPLY)
    {
        ndrx_TPset_error_msg(TPEINVAL, "TPNOREPLY cannot be used with tpcall()");
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tpcall (svc, idata, ilen, odata, olen, flags, NULL, 0, 0);
    
out:
    return ret;
}

/**
 * API version of tpgetrply
 * @param cd
 * @param data
 * @param len
 * @param flags
 * @return SUCCEED/FAIL
 */
expublic int tpgetrply (int *cd, char **data, long *len, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    if (cd==NULL)
    {
        ndrx_TPset_error_msg(TPEINVAL, "cd cannot be null");
        ret=EXFAIL;
        goto out;
    }

    if (data==NULL)
    {
        ndrx_TPset_error_msg(TPEINVAL, "data cannot be null");
        ret=EXFAIL;
        goto out;
    }

    if (len==NULL)
    {
        ndrx_TPset_error_msg(TPEINVAL, "len cannot be null");
        ret=EXFAIL;
        goto out;
    }

    if (flags & TPGETANY)
        ret=ndrx_tpgetrply (cd, EXFAIL, data, len, flags, NULL);
    else if (*cd <= 0 )
    {
        ndrx_TPset_error_msg(TPEINVAL, "*cd <= 0");
        ret=EXFAIL;
        goto out;
    }
    else
        ret=ndrx_tpgetrply (cd, *cd, data, len, flags, NULL);
        
out:
    return ret;
}

/**
 * API version of tpcall
 * @param svc
 * @param idata
 * @param ilen
 * @param odata
 * @param olen
 * @param flags
 * @param extradata - extra data to be passed over the call
 * @return SUCCEED/FAIL
 */
expublic int tpcallex (char *svc, char *idata, long ilen,
                char * *odata, long *olen, long flags,
                char *extradata, int dest_node, int ex_flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    /* Check some other parameters */
    if (olen==NULL)
    {
        ndrx_TPset_error_msg(TPEINVAL, "olen cannot be null");
        ret=EXFAIL;
        goto out;
    }
    
    /* Check some other parameters */
    if (odata==NULL)
    {
        ndrx_TPset_error_msg(TPEINVAL, "odata cannot be null");
        ret=EXFAIL;
        goto out;
    }
    
    if (flags & TPNOREPLY)
    {
        ndrx_TPset_error_msg(TPEINVAL, "TPNOREPLY cannot be used with tpcall()");
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tpcall (svc, idata, ilen, odata, olen, flags, extradata, dest_node, ex_flags);

out:
    return ret;
}

/**
 * Distributed transaction abort.
 * @return SUCCED/FAIL
 */
expublic int tpabort (long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;
    
    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpabort(flags);
    
out:
    return ret;
}

/**
 * Distributed transaction begin
 * @return SUCCEED/FAIL
 */
expublic int tpbegin (unsigned long timeout, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;
    
    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpbegin(timeout, flags);
    
out:
    return ret;
}

/**
 * Distro transaction commit
 * @return SUCCED/FAIL
 */
expublic int tpcommit (long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;
    
    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpcommit(flags);
    
out:
    return ret;
}

/**
 * Open XA interface
 * @return SUCCEED/FAIL
 */
expublic int tpopen (void)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;
    
    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpopen();
    
out:
    return ret;
}


/**
 * Close XA interface
 * @return
 */
expublic int tpclose (void)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;
    
    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpclose();
    
out:
    return ret;
}

/**
 * Return the current status in global tx or not
 * @return	0 - not int tx, 1 - in transaction
 */
expublic int tpgetlev (void)
{
    ndrx_TPunset_error(); /* this early does TLS entry */

    if (G_atmi_tls->G_atmi_xa_curtx.txinfo)
    {
        return 1;
    }

    return 0;
}


/**
 * API entry for tpcancel
 * @param cd
 * @return SUCCEED/FAIL
 */
expublic int tpcancel (int cd)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tpcancel (cd);

out:
    return ret;
}

/**
 * tpfree implementation
 * @param buf
 */
expublic void tpfree (char *buf)
{
    ndrx_TPunset_error();

    if (NULL!=buf)
    {
        ndrx_tpfree(buf, NULL);
    }
    else
    {
        NDRX_LOG(log_warn, "Trying to tpfree NULL buffer!");
    }
}

/**
 * tpisautobuf implementation
 * @param buf Typed buffer ptr
 * @return TRUE (automatically allocated buffer), FALSE(0) Manually allocated, -1 FAIL
 */
expublic int tpisautobuf (char *buf)
{
    ndrx_TPunset_error();

    if (NULL!=buf)
    {
        return ndrx_tpisautobuf(buf);
    }
    else
    {
        ndrx_TPset_error_msg(TPEINVAL, "Null buffer passed to tpisautobuf()!");
        return EXFAIL;
    }
}

/**
 * tpterm API entry.
 * We do not do standard initailization.
 * Intentionally we do not do API init here, because we do not wan't that
 * queue gets open for nothing (if this is first API call?)
 * Only not sure how about debug?
 * @return SUCCEED/FAIL
 */
expublic int tpterm (void)
{
    ndrx_TPunset_error();
    return ndrx_tpterm();
}

/**
 * API version of tpconnect
 * @param svc
 * @param data
 * @param len
 * @param flags
 * @return
 */
expublic int tpconnect (char *svc, char *data, long len, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpconnect (svc, data, len, flags);

out:
    return ret;
}

/**
 * API version of tprecv
 * @param cd
 * @param data
 * @param len
 * @param flags
 * @param revent
 * @param command_id
 * @return
 */
expublic int tprecv (int cd, char * *data,
                        long *len, long flags, long *revent)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    short command_id=ATMI_COMMAND_CONVDATA;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tprecv (cd, data, len, flags, revent, &command_id);

out:
    return ret;
}

/**
 * API version of tpsend
 * @param cd
 * @param data
 * @param len
 * @param flags
 * @param revent
 * @return
 */
expublic int tpsend (int cd, char *data, long len, long flags,
                                    long *revent)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tpsend (cd, data, len, flags, revent, ATMI_COMMAND_CONVDATA);

out:
    return ret;
}

/**
 * API version of tpdiscon
 * @param cd
 * @param data
 * @param len
 * @param flags
 * @param revent
 * @return
 */
expublic int tpdiscon (int cd)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tpdiscon (cd);

out:
    return ret;
}

/**
 * API version of tppost
 * @param eventname
 * @param data
 * @param len
 * @param flags
 * @return
 */
int tppost(char *eventname, char *data, long len, long flags)
{
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tppost(eventname, data, len, flags);

out:
    return ret;
}

/**
 * API entry for tpsubscribe
 */
long tpsubscribe(char *eventexpr, char *filter, TPEVCTL *ctl, long flags)
{
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tpsubscribe(eventexpr, filter, ctl, flags);

out:
    return ret;
}

/**
 * API version of tpunsubscribe
 */
int tpunsubscribe(long subscription, long flags)
{
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tpunsubscribe(subscription, flags);

out:
    return ret;
}


/**
 * Not supported, Just for build compliance.
 * @param 
 * @param 
 * @param 
 * @return 
 */
expublic int tpconvert (char *strrep, char *binrep, long flags)
{
    NDRX_LOG(log_error, "tpconvert - ATMI dummy");
    
    return EXSUCCEED;
}

/**
 * Suspend global XA transaction
 * @param tranid
 * @param flags
 * @return SUCCEED/FAIL
 */
expublic int tpsuspend (TPTRANID *tranid, long flags) 
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret = ndrx_tpsuspend (tranid, flags, EXFALSE);
    
out:
    return ret;
}

/**
 * Resume global XA transaction
 * @param tranid
 * @param flags
 * @return SUCCEED/FAIL
 */
expublic int tpresume (TPTRANID *tranid, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret = ndrx_tpresume (tranid, flags);
    
out:
    return ret;
}

/**
 * Return type information to caller.
 * The buffer must be allocated by tpalloc.
 * @param ptr
 * @param type
 * @param subtype
 * @return 
 */
expublic long tptypes (char *ptr, char *type, char *subtype)
{
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    if (ptr==NULL)
    {
        ndrx_TPset_error_msg(TPEINVAL, "ptr cannot be null");
        ret=EXFAIL;
        goto out;
    }

    ret=ndrx_tptypes(ptr, type, subtype);

out:
    return ret;
}


/**
 * Return current node id
 * @return FAIL = error, >0 node id.
 */
expublic long tpgetnodeid(void)
{
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret = G_atmi_env.our_nodeid;
    
out:
    return ret;
}

/**
 * Convert JSON to UBF
 * @param p_ub  output UBF
 * @param buffer    input buffer
 * @return 
 */
expublic int tpjsontoubf(UBFH *p_ub, char *buffer)
{
    return ndrx_tpjsontoubf(p_ub, buffer);
}

/**
 * Convert UBF to JSON text
 * @param p_ub
 * @param buffer
 * @param bufsize
 * @return 
 */
expublic int tpubftojson(UBFH *p_ub, char *buffer, int bufsize)
{
    return ndrx_tpubftojson(p_ub, buffer, bufsize);
}

/**
 * Enqueue message
 */
expublic int tpenqueue (char *qspace, char *qname, TPQCTL *ctl, char *data, long len, long flags)
{   
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpenqueue (qspace, 0, 0, qname, ctl, data, len, flags);

out:
    return ret;

}

/**
 * Dequeue message
 */
expublic int tpdequeue (char *qspace, char *qname, TPQCTL *ctl, char **data, long *len, long flags)
{
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpdequeue (qspace, 0, 0, qname, ctl, data, len, flags);

out:
    return ret;
}

/**
 * Enqueue message
 */
expublic int tpenqueueex (short nodeid, short srvid, char *qname, TPQCTL *ctl, char *data, long len, long flags)
{   
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpenqueue ("", nodeid, srvid, qname, ctl, data, len, flags);

out:
    return ret;

}

/**
 * Dequeue message
 */
expublic int tpdequeueex (short nodeid, short srvid, char *qname, TPQCTL *ctl, char **data, long *len, long flags)
{
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret=ndrx_tpdequeue ("", nodeid, srvid, qname, ctl, data, len, flags);

out:
    return ret;
}

/* Internal API: */

/**
 * For AIX we have issues with global variable exports
 * But functions work..
 * @return 
 */
expublic tp_command_call_t *ndrx_get_G_last_call(void)
{
    ATMI_TLS_ENTRY;
    
    return &G_atmi_tls->G_last_call;
}

/**
 * Access to atmi lib conf
 * @return 
 */
expublic atmi_lib_conf_t *ndrx_get_G_atmi_conf(void)
{
    ATMI_TLS_ENTRY;
    return &G_atmi_tls->G_atmi_conf;
}

/**
 * Access to to atmi lib env globals
 * @return 
 */
expublic atmi_lib_env_t *ndrx_get_G_atmi_env(void)
{
    return &G_atmi_env;
}

/**
 * Get current ATMI transaction object
 * @return 
 */
expublic atmi_xa_curtx_t *ndrx_get_G_atmi_xa_curtx(void)
{
    ATMI_TLS_ENTRY;
    return &G_atmi_tls->G_atmi_xa_curtx;
}

/**
 * Get accepted connection ATMI object
 * @return 
 */
expublic tp_conversation_control_t *ndrx_get_G_accepted_connection(void)
{
    ATMI_TLS_ENTRY;
    
    return &G_atmi_tls->G_accepted_connection;
}

/**
 * Kill the given context
 * @param context
 * @param flags
 * @return 
 */
expublic void tpfreectxt(TPCONTEXT_T context)
{
    ndrx_tpfreectxt(context);
}

/**
 * Set Enduro/X context for current thread
 * @param context
 * @param flags
 * @return 
 */
expublic int tpsetctxt(TPCONTEXT_T context, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    /* API_ENTRY; */

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret = ndrx_tpsetctxt(context, flags, (CTXT_PRIV_NSTD|CTXT_PRIV_UBF|
            CTXT_PRIV_ATMI|CTXT_PRIV_TRAN));
    
out:
    return ret;
}

/**
 * Get the current context
 * This disconnects current thread from TLS.
 * @param flags
 * @return 
 */
expublic int tpgetctxt(TPCONTEXT_T *context, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    /* API_ENTRY; */

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }
    
    ret = ndrx_tpgetctxt(context, flags, (CTXT_PRIV_NSTD|CTXT_PRIV_UBF|
            CTXT_PRIV_ATMI|CTXT_PRIV_TRAN));
    
out:
    return ret;
}


/**
 * Get the current context
 * This disconnects current thread from TLS.
 * We will allocate the NSTD & UBF TLSes too
 * @param flags
 * @return 
 */
expublic TPCONTEXT_T tpnewctxt(int auto_destroy, int auto_set)
{
    TPCONTEXT_T ctx = ndrx_atmi_tls_new(auto_destroy, auto_set);
    
    if (NULL!=ctx)
    {
        atmi_tls_t * ac = (atmi_tls_t *)ctx;
        
        ac->p_nstd_tls = ndrx_nstd_tls_new(auto_destroy, auto_set);
        ac->p_ubf_tls = ndrx_ubf_tls_new(auto_destroy, auto_set);
    }
    
    return ctx;
}

/**
 * Set the request logfile
 * - If data exists and filename exists, then update data buffer and set active 
 *      request logfile to filename
 * - If data does not exists, and filename exists, then set active request logfile
 *      to filename
 * @param data XATMI buffer
 * @param filename new logfile (if no data, then this is source for filename).
 * @param filesvc filename service (optional) - call the XATMI server for 
 *              generating file name and putting into buffer
 * @return SUCCEED/FAIL
 */
expublic int tplogsetreqfile(char **data, char *filename, char *filesvc)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        EXFAIL_OUT(ret);
    }
    
    ret = ndrx_tplogsetreqfile(data, filename, filesvc);
    
out:
    return ret;
}

/**
 * Print UBF buffer to logger
 * @param lev logging level to start print at
 * @param title title of the dump
 * @param p_ub UBF buffer
 */
expublic void tplogprintubf(int lev, char *title, UBFH *p_ub)
{
    ndrx_tplogprintubf(lev, title, p_ub);
}

/**
 * Get the filename from buffer ()
 * @param data
 * @param filename
 * @return SUCCEED (have filename in buffer, 
 */
expublic int tploggetbufreqfile(char *data, char *filename, int bufsize)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        EXFAIL_OUT(ret);
    }
    
    ret = ndrx_tploggetbufreqfile(data, filename, bufsize);
    
out:
    return ret;
}

/**
 * Delete the request file from buffer
 * @param data XATMI buffer
 * @return SUCCEED (have filename in buffer, 
 */
expublic int tplogdelbufreqfile(char *data)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        EXFAIL_OUT(ret);
    }
    
    ret = ndrx_tplogdelbufreqfile(data);
    
out:
    return ret;
}


/**
 * Admin call interface, not yet supported.
 * @param inbuf
 * @param outbuf
 * @param flags
 * @return 
 */
expublic int tpadmcall(UBFH *inbuf, UBFH **outbuf, long flags)
{
    long ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }   
    ndrx_TPset_error_msg(TPENOENT, "TODO: tpadmcall: Not yet implemented.");
    ret = EXFAIL;

out:
    return ret;
}

/**
 * STUB for tpchkauth()
 * @return FAIL
 */
expublic int tpchkauth(void)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        ret=EXFAIL;
        goto out;
    }   
    ndrx_TPset_error_msg(TPENOENT, "TODO: tpchkauth: Not yet implemented.");
    ret = EXFAIL;

out:
    return ret;
}


/**
 * STUB for tpnotify()
 * @param clientid
 * @param data
 * @param len
 * @param flags
 * @return FAIL
 */
expublic int tpnotify(CLIENTID *clientid, char *data, long len, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    TPMYID myid;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        EXFAIL_OUT(ret);
    }   
    
    if (NULL==clientid)
    {
        NDRX_LOG(log_error, "%s: clientid is NULL!", __func__);
        ndrx_TPset_error_msg(TPEINVAL, "clientid is NULL!");
        
        EXFAIL_OUT(ret);
    }
    
    if (EXSUCCEED!=ndrx_myid_parse(clientid->clientdata, &myid, EXFALSE))
    {
        NDRX_LOG(log_error, "%s: Failed to parse my_id!", __func__);
        ndrx_TPset_error_fmt(TPEINVAL, "Failed to parse: [%s]", clientid->clientdata);
        
        EXFAIL_OUT(ret);
    }
       
    if (EXSUCCEED!=ndrx_tpnotify(clientid, &myid, NULL, data, len, flags,
            myid.nodeid, NULL, NULL, NULL, 0L))
    {
        NDRX_LOG(log_error, "_tpnotify - failed!");
        EXFAIL_OUT(ret);
    }

out:

    NDRX_LOG(log_error, "%s returns %d", __func__, ret);

    return ret;
}

/**
 * Set handler for unsolicited messages
 * @param disp message hander (if processing allowed) or NULL (if no delivery needed)
 * @return previous handler
 */
expublic void (*tpsetunsol (void (*disp) (char *data, long len, long flags))) (char *data, long len, long flags)
{
    void * ret=NULL;
    int entry_status=EXSUCCEED;
    API_ENTRY;
            
    if (EXSUCCEED!=entry_status)
    {
        ret=TPUNSOLERR;
        goto out;
    }
    
    ret = G_atmi_tls->p_unsol_handler;
    
    G_atmi_tls->p_unsol_handler = disp;
    
    NDRX_LOG(log_debug, "%s: new disp=%p old=%p", 
            __func__, G_atmi_tls->p_unsol_handler, ret);

out:
    return (void (*) (char *, long, long))ret;
}

/**
 * Return constant for tpsetunsol()
 * @param data
 * @param len
 * @param flags
 */
expublic void ndrx_ndrx_tmunsolerr_handler (char *data, long len, long flags)
{
    NDRX_LOG(log_debug, "ndrx_ndrx_tmunsolerr_handler() - TPUNSOLERR called");
}

/**
 * Check unsolicited messages by client
 * @return FAIL
 */
expublic int tpchkunsol(void) 
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        EXFAIL_OUT(ret);
    }   
    
    if (ndrx_tpchkunsol()<0)
    {
        NDRX_LOG(log_error, "ndrx_tpchkunsol failed");
        EXFAIL_OUT(ret);
    }

out:
    return ret;
}

/**
 * Broadcast the message to matched clients
 * @param lmid optional nodeid can be NULL (matches all), emtpy (matches all), 
 * or exact match or regexp match.
 * @param usrname not used can be null or set or empty
 * @param cltname client name can be NULL or empty (matches all). Or regexp match
 * @param data tpalloc allocated buffer
 * @param len data len
 * @param flags flags like TPNOBLOCK and TPREGEXMATCH
 * @return SUCCEED/FAIL
 */
expublic int tpbroadcast(char *lmid, char *usrname, char *cltname,
  char *data,  long len, long flags)
{
    int ret=EXSUCCEED;
    int entry_status=EXSUCCEED;
    API_ENTRY;

    if (EXSUCCEED!=entry_status)
    {
        EXFAIL_OUT(ret);
    }   
    
    if (EXSUCCEED!=ndrx_tpbroadcast_local(lmid, usrname, cltname, 
            data,  len, flags, 0))
    {
        NDRX_LOG(log_error, "ndrx_tpbroadcast_local failed");
        EXFAIL_OUT(ret);
    }

out:
    return ret;
}

