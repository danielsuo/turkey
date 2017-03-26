function (
  build_flatcc
  schemas
  custom_target_name
  additional_dependencies
  generated_includes_dir
)
  set (FLATCC ${FLATCC_EXECUTABLE})
  set (all_generated_files "")

  foreach (schema ${schemas})
    get_filename_component (filename ${schema} NAME_WE)
    set (generated_include ${generated_includes_dir}/${filename}_generated_cc.h)
    add_custom_command (
      OUTPUT ${generated_include}
      COMMAND ${FLATCC}
        -o ${generated_includes_dir}
        -a ${schema}
      COMMENT "Building C header for ${schema}"
    )
    list(APPEND all_generated_files ${generated_include})
  endforeach ()

  message(${custom_target_name})
  add_custom_target(${custom_target_name} ALL DEPENDS ${all_generated_files} ${additional_dependencies})

endfunction ()
