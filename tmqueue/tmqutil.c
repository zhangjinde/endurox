/* 
** Q util
**
** @file xasrvutil.c
** 
** -----------------------------------------------------------------------------
** Enduro/X Middleware Platform for Distributed Transaction Processing
** Copyright (C) 2015, ATR Baltic, SIA. All Rights Reserved.
** This software is released under one of the following licenses:
** GPL or ATR Baltic's license for commercial use.
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
** A commercial use license is available from ATR Baltic, SIA
** contact@atrbaltic.com
** -----------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <utlist.h>

#include <ndebug.h>
#include <atmi.h>
#include <atmi_int.h>
#include <typed_buf.h>
#include <ndrstandard.h>
#include <ubf.h>
#include <Exfields.h>

#include <exnet.h>
#include <ndrxdcmn.h>

#include "tmqueue.h"
#include "../libatmisrv/srv_int.h"
#include <uuid/uuid.h>
#include <xa_cmn.h>
#include <atmi_int.h>
/*---------------------------Externs------------------------------------*/
/*---------------------------Macros-------------------------------------*/
/*---------------------------Enums--------------------------------------*/
/*---------------------------Typedefs-----------------------------------*/
/*---------------------------Globals------------------------------------*/
/*---------------------------Statics------------------------------------*/
MUTEX_LOCKDECL(M_msgid_gen_lock); /* Thread locking for xid generation    */
/*---------------------------Prototypes---------------------------------*/

/**
 * Generate new transaction id, native form (byte array)
 * Note this initializes the msgid.
 * @param xid
 */
public void tmq_msgid(char *msgid)
{
    uuid_t uuid_val;
    short node_id = (short) G_atmi_env.our_nodeid;
    short srv_id = (short) G_srv_id;
   
    memset(msgid, 0, TMMSGIDLEN);
    
    /* Do the locking, so that we get unique xids... */
    MUTEX_LOCK_V(M_msgid_gen_lock);
    uuid_generate(uuid_val);
    MUTEX_UNLOCK_V(M_msgid_gen_lock);
    
    memcpy(msgid, uuid_val, sizeof(uuid_t));
    /* Have an additional infos for transaction id... */
    memcpy(msgid  
            +sizeof(uuid_t)  
            ,(char *)&(node_id), sizeof(short));
    memcpy(msgid  
            +sizeof(uuid_t) 
            +sizeof(short)
            ,(char *)&(srv_id), sizeof(short));    
    
    NDRX_LOG(log_error, "MSGID: struct size: %d", sizeof(uuid_t)+sizeof(short)+ sizeof(short));
}



/* Get msgidstr from msgid */

/* Get msgid from msgidstr */


