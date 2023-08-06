static inline void cvtyuvtoRGB (SwsContext *c,

			   vector signed short Y, vector signed short U, vector signed short V,

			   vector signed short *R, vector signed short *G, vector signed short *B)

{

  vector signed   short vx,ux,uvx;



  Y = vec_mradds (Y, c->CY, c->OY);



  U = vec_sub (U,(vector signed short)(128));

  V = vec_sub (V,(vector signed short)(128));



  

  ux = vec_sl (U, c->CSHIFT);

  *B = vec_mradds (ux, c->CBU, Y);



  

  vx = vec_sl (V, c->CSHIFT);

  *R = vec_mradds (vx, c->CRV, Y);



  

  uvx = vec_mradds (U, c->CGU, Y);

  *G = vec_mradds (V, c->CGV, uvx);

}