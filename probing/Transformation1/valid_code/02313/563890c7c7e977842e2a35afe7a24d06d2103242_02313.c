QDict *qtest_qmpv(QTestState *s, const char *fmt, va_list ap)

{

    

    socket_sendf(s->qmp_fd, fmt, ap);



    

    return qtest_qmp_receive(s);

}