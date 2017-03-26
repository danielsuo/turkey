#include "common.h"

struct turkey_shm *turkey_shm_init(pid_t pid) {
  struct turkey_shm *tshm;

  if ((tshm = (struct turkey_shm *)malloc(sizeof(struct turkey_shm *))) == NULL) {
    pexit("Failed to allocate memory for shared memory struct");
  }

  tshm->pid = pid;
  tshm->data = turkey_data_init();

  if ((tshm->shm_key_path = tsprintf(TURKEY_SHM_PATH_FORMAT, tshm->pid)) == NULL) {
    pexit("Failed to create shared memory path");
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
  free(tshm->shm_key_path);
  if ((tshm->shm_key_path = tsprintf(TURKEY_SHM_PATH_FORMAT, tshm->pid)) == NULL) {
    pexit("Failed to create shared memory path");
  }

  if (remove(tshm->shm_key_path) < 0) {
    pexit("Failed to remove shared memory file");
  }

  turkey_data_destroy(tshm->data);
  free(tshm->shm_key_path);
  free(tshm);
}

int turkey_shm_read(struct turkey_shm *tshm, void *buffer, size_t size) {
  if (turkey_shm_lock(tshm) < 0) {
    perror("Failed to lock shared memory");
    return -1;
  }
  memcpy(buffer, tshm->shm, size);
  if (turkey_shm_unlock(tshm) < 0) {
    perror("Failed to unlock shared memory");
    return -1;
  }
}

int turkey_shm_write(struct turkey_shm *tshm, void *buffer, size_t size) {
  if (turkey_shm_lock(tshm) < 0) {
    perror("Failed to lock shared memory");
    return -1;
  }
  memcpy(tshm->shm, buffer, size);
  if (turkey_shm_unlock(tshm) < 0) {
    perror("Failed to unlock shared memory");
    return -1;
  }
}

int turkey_data_read(struct turkey_shm *tshm) {
  Turkey_turkey_shm_data_table_t table = Turkey_turkey_shm_data_as_root(tshm->shm);
  tshm->data->cpu_shares = Turkey_turkey_shm_data_cpu_shares(table);
  tshm->data->cpid = Turkey_turkey_shm_data_cpid(table);
  tshm->data->spid = Turkey_turkey_shm_data_spid(table);

  return 0;
}

int turkey_data_write(struct turkey_shm *tshm) {
  void *buffer;
  size_t size;
  flatcc_builder_t builder, *B;
  B = &builder;
  flatcc_builder_init(B);

  Turkey_turkey_shm_data_create_as_root(B,
    tshm->data->cpid, tshm->data->spid, tshm->data->cpu_shares);

  buffer = flatcc_builder_finalize_aligned_buffer(B, &size);

  if (turkey_shm_write(tshm, buffer, size) < 0) {
    pexit("Failed to write data to shm");
  }

  flatcc_builder_clear(B);
}

int turkey_shm_lock(struct turkey_shm *tshm) {
  return shmctl(tshm->shm_id, SHM_LOCK, NULL);
}

int turkey_shm_unlock(struct turkey_shm *tshm) {
  return shmctl(tshm->shm_id, SHM_UNLOCK, NULL);
}

struct turkey_data *turkey_data_init() {
  struct turkey_data *tdata;

  if ((tdata = (struct turkey_data *)malloc(sizeof(struct turkey_data *))) == NULL) {
    pexit("Failed to allocate memory for shared memory struct");
  }

  return tdata;
}

void turkey_data_destroy(struct turkey_data *tdata) {
  free(tdata);
}
