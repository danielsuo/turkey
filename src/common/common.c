#include "common.h"

struct turkey_shm *turkey_shm_init(int pid) {
  struct turkey_shm *tshm;

  if ((tshm = (struct turkey_shm *)malloc(sizeof(struct turkey_shm *))) == NULL) {
    pexit("Failed to allocate memory for shared memory struct");
  }

  tshm->pid = pid;

  if ((tshm->shm_key_path_len =
           snprintf(NULL, 0, TURKEY_SHM_PATH_FORMAT, tshm->pid)) < 0) {
    pexit("Failed to get shared memory path length");
  }

  if ((tshm->shm_key_path = (char *)malloc(++tshm->shm_key_path_len)) ==
      NULL) {
    pexit("Failed to allocate memory for shared memory file name");
  }

  if (snprintf(tshm->shm_key_path, tshm->shm_key_path_len,
               TURKEY_SHM_PATH_FORMAT, tshm->pid) < 0) {
    pexit("Failed to create shared memory file name string");
  }

  fprintf(stderr, "%d -> %s\n", tshm->pid, tshm->shm_key_path);

  FILE *shm_file;
  if (!(shm_file = fopen(tshm->shm_key_path, "wt"))) {
    pexit("Failed to create file for shared memory");
  }
  fclose(shm_file);

  if ((tshm->shm_key = ftok(tshm->shm_key_path, 1)) == -1) {
    pexit("Failed to get IPC key from file name");
  }

  if ((tshm->shm_id = shmget(tshm->shm_key, TURKEY_SHM_SIZE, IPC_CREAT | 0666)) < 0) {
    pexit("Failed to create the segment");
  }

  if ((tshm->shm = (unsigned char *)shmat(tshm->shm_id, NULL, 0)) == (unsigned char *)-1) {
    pexit("Failed to attach to data space");
  }

  return tshm;
}

void turkey_shm_destroy(struct turkey_shm *tshm) {
  if (shmdt(tshm->shm) < 0) {
    pexit("Failed to detach from shared memory");
  }

  if (shmctl(tshm->shm_id, IPC_RMID, NULL) < 0) {
    pexit("Failed to remove shared memory");
  }

  // TODO: shm_key_path gets emptied
  snprintf(tshm->shm_key_path, tshm->shm_key_path_len,
               TURKEY_SHM_PATH_FORMAT, tshm->pid);
  if (remove(tshm->shm_key_path) < 0) {
    pexit("Failed to remove shared memory file");
  }

  free(tshm->shm_key_path);
  free(tshm);
}
