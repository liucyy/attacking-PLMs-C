const char *path(const char *name)

{

    

    if (!base || name[0] != '/')

	return name;



    return follow_path(base, name) ?: name;

}