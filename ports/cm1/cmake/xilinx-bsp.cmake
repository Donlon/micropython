function(xil_bsp_add_directory_source target_name path)
    file(GLOB comp_src ${path}/*.c)
    file(GLOB comp_inc ${path}/*.h)

    set(comp_src_new "")
    foreach (src ${comp_src})
        if (src MATCHES "^.*_g.c$")
            continue()
        endif()
        if (src MATCHES "^.*_sinit.c$")
            continue()
        endif()
        list(APPEND comp_src_new ${src})
    endforeach ()

        message(${comp_src_new})
    target_sources(${target_name} PUBLIC ${comp_src_new})

    foreach (inc ${comp_inc})
        get_filename_component(inc_filename ${inc} NAME)
        configure_file(${inc} ${include_dir}/${inc_filename} COPYONLY)
    endforeach ()
endfunction()

function(create_xilinx_bsp target_name platform components)
    set(include_dir "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_includes/")
    set(${target_name}_include_dir "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_includes/" PARENT_SCOPE)
    add_library(${target_name})
    target_include_directories(${target_name} PRIVATE ${include_dir})

    file(WRITE ${include_dir}/bspconfig.h "")
#    file(WRITE ${include_dir}/xparameters.h "")
    file(WRITE ${include_dir}/xpseudo_asm.h "")

    xil_bsp_add_directory_source(${target_name} ${XILINX_LIB_PATH}/lib/bsp/standalone/src/common)

#    list(APPEND components "common")
    foreach (component ${components})
        if (NOT EXISTS "${XILINX_LIB_PATH}/XilinxProcessorIPLib/drivers/${component}"
                OR NOT EXISTS "${XILINX_LIB_PATH}/XilinxProcessorIPLib/drivers/${component}/data/${component}.mdd")
            message(WARNING "Library ${component} is not found")
            continue()
        endif ()

        set(comp_path "${XILINX_LIB_PATH}/XilinxProcessorIPLib/drivers/${component}/src")

        xil_bsp_add_directory_source(${target_name} ${comp_path})
    endforeach ()
endfunction()