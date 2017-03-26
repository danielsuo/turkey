#ifndef TURKEY_FBS_H
#define TURKEY_FBS_H

#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(Turkey, x)

#ifdef __cplusplus
  #include "turkey_msg_register_client_generated.h"
#else
  #include "turkey_msg_register_client_builder.h"
  #include "turkey_msg_register_client_reader.h"
  #include "turkey_msg_register_client_verifier.h"
#endif

#ifdef __cplusplus
  #include "turkey_shm_data_generated.h"
#else
  #include "turkey_shm_data_builder.h"
  #include "turkey_shm_data_reader.h"
  #include "turkey_shm_data_verifier.h"
#endif

#endif // TURKEY_FBS_H
