def queue_task():
    mysched.queue_task('test', repeats=0, period=60)
