typedef  struct 
{
	pthread_mutex_t  rw_mutex;
	pthread_cond_t   rw_condreaders;
	pthread_cond_t   rw_condwriters;
	int      rw_magic;
	int      rw_nwaitreaders;
	int      rw_nwaitwriters;
	int      rw_refcount;

}pthread_rwlock_t;

int  pthread_rwlock_init(pthread_rwlock_t *rw,  pthread_rwlockattr_t *attr)
{
	int   result;

	if(attr != NULL)
		return (EINVAL);

	if((result = pthread_mutex_init(&rw->rw_mutex, NULL)) != 0)
		goto err1;

	if((result = pthread_cond_init(&rw->rw_condreaders, NULL)) != 0)
		goto err2;

	if((result = pthread_cond_init(&rw->rw_condwriters, NULL)) != 0)
		goto  err3;
	rw->rw_nwaitreaders = 0;
	rw->rw_nwaitwriters = 0;
	rw->rw_refcount = 0;
	rw->rw_magic = RW_MAGIC;

	return (0);

	err3:
	pthread_cond_destroy(&rw->rw_condreaders);
	err2:
	pthread_cond_destroy(&rw->rw_condwriters);
	err1:
	return (result);
}

int  pthread_rwlock_rdlock(pthread_rwlock_t *rw)
{
	int  result;

	if(rw->rw_magic != RW_MAGIC)
		return (EINVAL);
	if((result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return (result);

	while(rw->rw_refcount < 0 || rw->rw_nwaitwriters > 0)
	{
		rw->rw_nwaitreaders++;
		result = pthread_cond_wait(&rw->rw_condreaders, &rw->rw_mutex);
		rw->rw_nwaitreaders--;
		if(result != 0)
			break;
	}
	if(result == 0)
		rw->rw_refcount++;

	pthread_mutex_unlock(&rw->rw_mutex);
	return (result);
}

int  pthread_rwlock_trylock(pthread_rwlock_t *rw)
{
	int  result;
	if(rw->rw_magic != RW_MAGIC)
		return (EINVAL);
	if((result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return (result);

	if(rw->rw_refcount < 0 || rw->rw_nwaitwriters > 0)
		result = EBUSY;
	else
		rw->rw_refcount++;
	pthread_mutex_unlock(&rw_mutex);
	return (result);
}


