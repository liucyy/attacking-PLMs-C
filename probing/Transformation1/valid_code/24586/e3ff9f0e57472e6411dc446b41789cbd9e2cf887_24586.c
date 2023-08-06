static void test_to_from_buf_1(void)

{

     unsigned niov;

     struct iovec *iov;

     size_t sz;

     unsigned char *ibuf, *obuf;

     unsigned i, j, n;



     iov_random(&iov, &niov);



     sz = iov_size(iov, niov);



     ibuf = g_malloc(sz + 8) + 4;

     memcpy(ibuf-4, "aaaa", 4); memcpy(ibuf + sz, "bbbb", 4);

     obuf = g_malloc(sz + 8) + 4;

     memcpy(obuf-4, "xxxx", 4); memcpy(obuf + sz, "yyyy", 4);



     

     for (i = 0; i < sz; ++i) {

         ibuf[i] = i & 255;

     }



     for (i = 0; i <= sz; ++i) {



         



         

         n = iov_memset(iov, niov, 0, 0xff, -1);

         g_assert(n == sz);



         

         n = iov_from_buf(iov, niov, i, ibuf + i, -1);

         g_assert(n == sz - i);



         

         memset(obuf + i, 0, sz - i);

         

         n = iov_to_buf(iov, niov, i, obuf + i, -1);

         g_assert(n == sz - i);



         

         g_assert(memcmp(ibuf, obuf, sz) == 0);



         

         n = iov_to_buf(iov, niov, i, obuf + i, 1);

         g_assert(n == (i < sz));

         if (n) {

             g_assert(obuf[i] == (i & 255));

         }



         for (j = i; j <= sz; ++j) {

             



             

             n = iov_memset(iov, niov, 0, 0xff, -1);

             g_assert(n == sz);



             

             n = iov_from_buf(iov, niov, i, ibuf + i, j - i);

             g_assert(n == j - i);



             

             memset(obuf + i, 0, j - i);



             

             n = iov_to_buf(iov, niov, i, obuf + i, j - i);

             g_assert(n == j - i);



             

             g_assert(memcmp(ibuf, obuf, sz) == 0);



             

             test_iov_bytes(iov, niov, i, j - i);

         }

    }

    g_assert(!memcmp(ibuf-4, "aaaa", 4) && !memcmp(ibuf+sz, "bbbb", 4));

    g_free(ibuf-4);

    g_assert(!memcmp(obuf-4, "xxxx", 4) && !memcmp(obuf+sz, "yyyy", 4));

    g_free(obuf-4);

    iov_free(iov, niov);

}