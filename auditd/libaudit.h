/*
 * Copyright 2012, Samsung Telecommunications of America
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Written by William Roberts <w.roberts@sta.samsung.com>
 */

#ifndef _LIBAUDIT_H_
#define _LIBAUDIT_H_

#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/audit.h>

#define MAX_AUDIT_MESSAGE_LENGTH    8970

typedef enum {
    GET_REPLY_BLOCKING=0,
    GET_REPLY_NONBLOCKING
} reply_t;

typedef enum {
    WAIT_NO,
    WAIT_YES
} rep_wait_t;

struct audit_sig_info {
    uid_t uid;
    pid_t pid;
    char ctx[0];
};

struct audit_message {
    struct nlmsghdr nlh;
    char   data[MAX_AUDIT_MESSAGE_LENGTH];
};

struct audit_reply {
    int                      type;
    int                      len;
    struct nlmsghdr         *nlh;
    struct audit_message     msg;

    union {
        struct audit_status     *status;
        struct audit_rule_data  *ruledata;
        const char              *message;
        struct nlmsgerr         *error;
        struct audit_sig_info   *signal_info;
    };
};

/**
 * Opens a connection to the Audit netlink socket
 * @return
 *  A valid fd on success or < 0 on error with errno set.
 *  Returns the same errors as man 2 socket.
 */
extern int  audit_open(void);

/**
 * Closes the fd returned from audit_open()
 * @param fd
 *  The fd to close
 */
extern void audit_close(int fd);

/**
 *
 * @param fd
 *  The fd returned by a call to audit_open()
 * @param rep
 *  The response struct to store the response in.
 * @param block
 *  Whether or not to block on IO
 * @param peek
 *  Whether or not we are to remove the message from
 *  the queue when we do a read on the netlink socket.
 * @return
 *  This function returns 0 on success, else -errno.
 */
extern int  audit_get_reply(int fd, struct audit_reply *rep, reply_t block,
               int peek);

/**
 * Sets a pid to recieve audit netlink events from the kernel
 * @param fd
 *  The fd returned by a call to audit_open()
 * @param pid
 *  The pid whom to set as the reciever of audit messages
 * @param wmode
 *  Whether or not to block on the underlying socket io calls.
 * @return
 *  This function returns 0 on success, -errno on error.
 */
extern int  audit_set_pid(int fd, uint32_t pid, rep_wait_t wmode);

#endif
