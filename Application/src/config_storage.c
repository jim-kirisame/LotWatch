#include "main.h"
#include "config_storage.h"
#include "pstorage.h"

watch_data watch_config_data;
pstorage_handle_t       pstorage_base_block_id;

void config_pstorage_callback_handler(pstorage_handle_t *p_handle, uint8_t op_code, uint32_t result, uint8_t *p_data, uint32_t data_len)
{
    switch(op_code)  
    {        
       case PSTORAGE_UPDATE_OP_CODE:  
           if (result == NRF_SUCCESS)  
           {  
               //my_flag = 1; //?flash update???????? Main???????flash???  
           }  
           else  
           {  
               // Update operation failed.  
           }  
           break;  
    }  
}

void config_pstorage_init(void)
{
    //pstorage init in device manager, so do not init here 
    
    
    //registe some parames
    pstorage_module_param_t param;
    uint32_t                err_code;
          
    param.block_size  = 128;    //size of config file is 94,
    param.block_count = 1;
    param.cb          = config_pstorage_callback_handler;
        
    err_code = pstorage_register(&param, &pstorage_base_block_id);
    APP_ERROR_CHECK(err_code);
    
}

void config_load_default(void)
{
    watch_config_data.persist.config.alarm_delay_max_count = 3;
    watch_config_data.persist.config.alarm_delay_time = 5;
    watch_config_data.persist.config.alarm_vibra_time = 10;
    watch_config_data.persist.config.debug_enable = true;
    watch_config_data.persist.config.display_sleep_time = 5;
    watch_config_data.persist.config.step_userdata.age = 18;
    watch_config_data.persist.config.step_userdata.height = 170;
    watch_config_data.persist.config.step_userdata.sex = 0;
    watch_config_data.persist.config.step_userdata.weight = 70;
}

void config_post_init()
{
    watch_config_data.temporary.disp_awake = true;
    watch_config_data.temporary.page_current_screen = 0x80;
}

void config_read(void)
{
    uint32_t err_code;
    uint32_t version;
    pstorage_handle_t block_handle;
    
    err_code = pstorage_block_identifier_get(&pstorage_base_block_id, 0, &block_handle);
    APP_ERROR_CHECK(err_code);
    
    pstorage_load((uint8_t *)&version, &block_handle, 4, 0);
    APP_ERROR_CHECK(err_code);
    if(version != PERSISTENT_DATA_VERSION)
    {
        config_load_default();
    }
    else
    {
        pstorage_load((uint8_t *)&watch_config_data.persist, &block_handle, sizeof(watch_persistent_data), 4);
        APP_ERROR_CHECK(err_code);
    }
}

void config_init(void)
{
    config_pstorage_init();
    config_read();
    config_post_init();
}

void config_write(void)
{
    uint32_t err_code;
    uint32_t version;
    pstorage_handle_t block_handle;
    
    version = PERSISTENT_DATA_VERSION;
    
    err_code = pstorage_block_identifier_get(&pstorage_base_block_id, 0, &block_handle);
    APP_ERROR_CHECK(err_code);
    
    err_code = pstorage_update(&block_handle,(uint8_t *)&version, 4, 0);
    APP_ERROR_CHECK(err_code);
    
    err_code =  pstorage_update(&block_handle,(uint8_t *)&watch_config_data.persist, sizeof(watch_persistent_data), 4);
    APP_ERROR_CHECK(err_code);
    
}

