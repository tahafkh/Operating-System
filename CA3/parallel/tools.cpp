void get_pixels_handler(int end, char *fileReadBuffer)
{
    int total_threads = THREAD_PER_DIM*THREAD_PER_DIM;
    pthread_t tids[total_threads];

    for(int i = 0; i < total_threads; i++){
        long id = i;
        int return_code = pthread_create(&tids[i], NULL, get_pixels, (void*)id);
    }


}