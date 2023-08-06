static void quorum_vote(QuorumAIOCB *acb)

{

    bool quorum = true;

    int i, j, ret;

    QuorumVoteValue hash;

    BDRVQuorumState *s = acb->common.bs->opaque;

    QuorumVoteVersion *winner;



    if (quorum_has_too_much_io_failed(acb)) {

        return;

    }



    

    for (i = 0; i < s->num_children; i++) {

        if (!acb->qcrs[i].ret) {

            break;

        }

    }



    assert(i < s->num_children);



    

    for (j = i + 1; j < s->num_children; j++) {

        if (acb->qcrs[j].ret) {

            continue;

        }

        quorum = quorum_compare(acb, &acb->qcrs[i].qiov, &acb->qcrs[j].qiov);

        if (!quorum) {

            break;

       }

    }



    

    if (quorum) {

        quorum_copy_qiov(acb->qiov, &acb->qcrs[i].qiov);

        return;

    }



    

    for (i = 0; i < s->num_children; i++) {

        if (acb->qcrs[i].ret) {

            continue;

        }

        ret = quorum_compute_hash(acb, i, &hash);

        

        if (ret < 0) {

            acb->vote_ret = ret;

            goto free_exit;

        }

        quorum_count_vote(&acb->votes, &hash, i);

    }



    

    winner = quorum_get_vote_winner(&acb->votes);



    

    if (winner->vote_count < s->threshold) {

        quorum_report_failure(acb);

        acb->vote_ret = -EIO;

        goto free_exit;

    }



    

    quorum_copy_qiov(acb->qiov, &acb->qcrs[winner->index].qiov);



    

    quorum_report_bad_versions(s, acb, &winner->value);



free_exit:

    

    quorum_free_vote_list(&acb->votes);

}