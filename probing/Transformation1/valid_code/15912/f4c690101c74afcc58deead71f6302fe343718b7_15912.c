static int sys_openat(int dirfd, const char *pathname, int flags, ...)

{

  

  if ((flags & O_CREAT) != 0) {

      va_list ap;

      mode_t mode;



      

      va_start(ap, flags);

      mode = va_arg(ap, mode_t);

      mode = target_to_host_bitmask(mode, fcntl_flags_tbl);

      va_end(ap);



      return (openat(dirfd, pathname, flags, mode));

  }

  return (openat(dirfd, pathname, flags));

}