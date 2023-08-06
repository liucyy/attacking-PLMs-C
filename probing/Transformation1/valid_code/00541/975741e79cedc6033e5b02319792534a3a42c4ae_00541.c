static int vorbis_residue_decode(vorbis_context *vc, vorbis_residue *vr, uint_fast8_t ch, uint_fast8_t *do_not_decode, float *vec, uint_fast16_t vlen) {

    GetBitContext *gb=&vc->gb;

    uint_fast8_t c_p_c=vc->codebooks[vr->classbook].dimensions;

    uint_fast16_t n_to_read=vr->end-vr->begin;

    uint_fast16_t ptns_to_read=n_to_read/vr->partition_size;

    uint_fast8_t classifs[ptns_to_read*vc->audio_channels];

    uint_fast8_t pass;

    uint_fast8_t ch_used;

    uint_fast8_t i,j,l;

    uint_fast16_t k;



    if (vr->type==2) {

        for(j=1;j<ch;++j) {

                do_not_decode[0]&=do_not_decode[j];  

        }

        if (do_not_decode[0]) return 0;

        ch_used=1;

    } else {

        ch_used=ch;

    }



    AV_DEBUG(" residue type 0/1/2 decode begin, ch: %d  cpc %d  \n", ch, c_p_c);



    for(pass=0;pass<=vr->maxpass;++pass) { 

        uint_fast16_t voffset;

        uint_fast16_t partition_count;

        uint_fast16_t j_times_ptns_to_read;



        voffset=vr->begin;

        for(partition_count=0;partition_count<ptns_to_read;) {  

            if (!pass) {

                uint_fast32_t inverse_class = ff_inverse[vr->classifications];

                for(j_times_ptns_to_read=0, j=0;j<ch_used;++j) {

                    if (!do_not_decode[j]) {

                        uint_fast32_t temp=get_vlc2(gb, vc->codebooks[vr->classbook].vlc.table,

                        vc->codebooks[vr->classbook].nb_bits, 3);



                        AV_DEBUG("Classword: %d \n", temp);



                        assert(vr->classifications > 1 && temp<=65536); 

                        for(i=0;i<c_p_c;++i) {

                            uint_fast32_t temp2;



                            temp2=(((uint_fast64_t)temp) * inverse_class)>>32;

                            if (partition_count+c_p_c-1-i < ptns_to_read) {

                                classifs[j_times_ptns_to_read+partition_count+c_p_c-1-i]=temp-temp2*vr->classifications;

                            }

                            temp=temp2;

                        }

                    }

                    j_times_ptns_to_read+=ptns_to_read;

                }

            }

            for(i=0;(i<c_p_c) && (partition_count<ptns_to_read);++i) {

                for(j_times_ptns_to_read=0, j=0;j<ch_used;++j) {

                    uint_fast16_t voffs;



                    if (!do_not_decode[j]) {

                        uint_fast8_t vqclass=classifs[j_times_ptns_to_read+partition_count];

                        int_fast16_t vqbook=vr->books[vqclass][pass];



                        if (vqbook>=0) {

                            uint_fast16_t coffs;

                            unsigned dim= vc->codebooks[vqbook].dimensions; 

                            uint_fast16_t step= dim==1 ? vr->partition_size

                                              : FASTDIV(vr->partition_size, dim);

                            vorbis_codebook codebook= vc->codebooks[vqbook];



                            if (vr->type==0) {



                                voffs=voffset+j*vlen;

                                for(k=0;k<step;++k) {

                                    coffs=get_vlc2(gb, codebook.vlc.table, codebook.nb_bits, 3) * dim;

                                    for(l=0;l<dim;++l) {

                                        vec[voffs+k+l*step]+=codebook.codevectors[coffs+l];  

                                    }

                                }

                            }

                            else if (vr->type==1) {

                                voffs=voffset+j*vlen;

                                for(k=0;k<step;++k) {

                                    coffs=get_vlc2(gb, codebook.vlc.table, codebook.nb_bits, 3) * dim;

                                    for(l=0;l<dim;++l, ++voffs) {

                                        vec[voffs]+=codebook.codevectors[coffs+l];  



                                        AV_DEBUG(" pass %d offs: %d curr: %f change: %f cv offs.: %d  \n", pass, voffs, vec[voffs], codebook.codevectors[coffs+l], coffs);

                                    }

                                }

                            }

                            else if (vr->type==2 && ch==2 && (voffset&1)==0 && (dim&1)==0) { 

                                voffs=voffset>>1;



                                if(dim==2) {

                                    for(k=0;k<step;++k) {

                                        coffs=get_vlc2(gb, codebook.vlc.table, codebook.nb_bits, 3) * 2;

                                        vec[voffs+k     ]+=codebook.codevectors[coffs  ];  

                                        vec[voffs+k+vlen]+=codebook.codevectors[coffs+1];  

                                    }

                                } else

                                for(k=0;k<step;++k) {

                                    coffs=get_vlc2(gb, codebook.vlc.table, codebook.nb_bits, 3) * dim;

                                    for(l=0;l<dim;l+=2, voffs++) {

                                        vec[voffs     ]+=codebook.codevectors[coffs+l  ];  

                                        vec[voffs+vlen]+=codebook.codevectors[coffs+l+1];  



                                        AV_DEBUG(" pass %d offs: %d curr: %f change: %f cv offs.: %d+%d  \n", pass, voffset/ch+(voffs%ch)*vlen, vec[voffset/ch+(voffs%ch)*vlen], codebook.codevectors[coffs+l], coffs, l);

                                    }

                                }



                            }

                            else if (vr->type==2) {

                                voffs=voffset;



                                for(k=0;k<step;++k) {

                                    coffs=get_vlc2(gb, codebook.vlc.table, codebook.nb_bits, 3) * dim;

                                    for(l=0;l<dim;++l, ++voffs) {

                                        vec[voffs/ch+(voffs%ch)*vlen]+=codebook.codevectors[coffs+l];  



                                        AV_DEBUG(" pass %d offs: %d curr: %f change: %f cv offs.: %d+%d  \n", pass, voffset/ch+(voffs%ch)*vlen, vec[voffset/ch+(voffs%ch)*vlen], codebook.codevectors[coffs+l], coffs, l);

                                    }

                                }

                            } else {

                                av_log(vc->avccontext, AV_LOG_ERROR, " Invalid residue type while residue decode?! \n");

                                return 1;

                            }

                        }

                    }

                    j_times_ptns_to_read+=ptns_to_read;

                }

                ++partition_count;

                voffset+=vr->partition_size;

            }

        }

    }

    return 0;

}