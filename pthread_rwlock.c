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