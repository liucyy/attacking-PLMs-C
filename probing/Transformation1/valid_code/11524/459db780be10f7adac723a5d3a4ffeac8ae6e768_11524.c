envlist_parse(envlist_t *envlist, const char *env,

    int (*callback)(envlist_t *, const char *))

{

	char *tmpenv, *envvar;

	char *envsave = NULL;



	assert(callback != NULL);



	if ((envlist == NULL) || (env == NULL))

		return (EINVAL);



	

	if ((tmpenv = strdup(env)) == NULL)

		return (errno);



	envvar = strtok_r(tmpenv, ",", &envsave);

	while (envvar != NULL) {

		if ((*callback)(envlist, envvar) != 0) {

			free(tmpenv);

			return (errno);

		}

		envvar = strtok_r(NULL, ",", &envsave);

	}



	free(tmpenv);

	return (0);

}