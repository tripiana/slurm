/* 
 * submit.c - submit a job with supplied contraints
 * see slurm.h for documentation on external functions and data structures
 *
 * author: moe jette, jette@llnl.gov
 */

#define DEBUG_SYSTEM 1

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <errno.h>
#include <stdio.h>

#include <src/api/slurm.h>
#include <src/common/slurm_protocol_api.h>


/* slurm_submit_job - load the supplied node information buffer if changed */
int
slurm_submit_batch_job (job_desc_msg_t * job_desc_msg )
{
        int msg_size ;
        int rc ;
        slurm_fd sockfd ;
        slurm_msg_t request_msg ;
        slurm_msg_t response_msg ;
        return_code_msg_t * slurm_rc_msg ;
	resource_allocation_response_msg_t * slurm_aloc_resp_msg;

        /* init message connection for message communication with controller */
        if ( ( sockfd = slurm_open_controller_conn ( ) ) == SLURM_SOCKET_ERROR )
                return SLURM_SOCKET_ERROR ;


        /* send request message */
        request_msg . msg_type = REQUEST_SUBMIT_BATCH_JOB ;
        request_msg . data = job_desc_msg ; 
        if ( ( rc = slurm_send_controller_msg ( sockfd , & request_msg ) ) == SLURM_SOCKET_ERROR )
                return SLURM_SOCKET_ERROR ;

        /* receive message */
        if ( ( msg_size = slurm_receive_msg ( sockfd , & response_msg ) ) == SLURM_SOCKET_ERROR )
                return SLURM_SOCKET_ERROR ;
        /* shutdown message connection */
        if ( ( rc = slurm_shutdown_msg_conn ( sockfd ) ) == SLURM_SOCKET_ERROR )
                return SLURM_SOCKET_ERROR ;

        switch ( response_msg . msg_type )
        {
                case RESPONSE_SLURM_RC:
                        slurm_rc_msg = ( return_code_msg_t * ) response_msg . data ;
			return (int) slurm_rc_msg->return_code ;
                        break ;
		case RESPONSE_SUBMIT_BATCH_JOB:
                        slurm_aloc_resp_msg = ( resource_allocation_response_msg_t * ) response_msg . data ;
			job_desc_msg->job_id = slurm_aloc_resp_msg->job_id;
			return 0;
			break;
                default:
                        return SLURM_UNEXPECTED_MSG_ERROR ;
                        break ;
        }

        return SLURM_SUCCESS ;
}
