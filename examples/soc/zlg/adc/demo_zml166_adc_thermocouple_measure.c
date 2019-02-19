/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief AM_ZML166_ADC�����̶���ѹ��ͨ����׼�ӿ�ʵ��
 *
 * - ʵ������
 *   1. ���Ӻô��ڣ���PT100�������TCB��TCA֮�䡣
 *   2. ���ڽ����ӡ���ȵ�ż�����¶ȡ�
 *
 * \par Դ����
 * \snippet dome_zml166_adc_pt100_measure.c src_demo_zml166_adc_thermistor_measure
 *
 * \internal
 * \par Modification History
 * - 1.00 18-12-28  htf, first implementation
 * \endinternal
 */

/**
 * \addtogroup demo_if_demo_zml166_adc_thermistor_measure
 * \copydoc demo_zml166_adc_thermistor_measure.c
 */

#include "am_zml166_adc.h"
#include "ametal.h"
#include "am_vdebug.h"
#include "am_delay.h"
#include "am_thermocouple.h"
#include "math.h"
/**
 * \brief ����AML166����ȵ�ż�����¶�����
 */
void dome_zml166_adc_thermocouple_measure_entry(am_zml166_adc_handle_t  handle,
                                                float                  *p_para,
                                                uint8_t                 type)
{

    int32_t  adc_val[1];
    double   temp;

    am_adc_handle_t     adc_handle  = &handle->adc_serve;

    while(1){
        double    temperature, v1;
        /* ��������¶� */
        am_zml166_adc_gain_set(handle, 1);
        am_zml166_adc_reg_set(handle,
                              AM_ZML166_ADC_ADC3_ADDR,
                              AM_ZML166_ADC_LVSHIFT_ENABLE|AM_ZML166_ADC_LVSCP_ENABLE);
        am_zml166_adc_mux_set(handle, AM_ZML166_ADC_INPS_AIN2 | AM_ZML166_ADC_INNS_GND);
        am_adc_read(adc_handle, 0, (uint32_t *)adc_val, AM_NELEMENTS(adc_val));

        temp  = (double)((double)adc_val[0] / 8388607 * 2500 );
        temp  = temp * p_para[0] + p_para[1];
        temp = (double)((temp * 8388607 / 2500 ) / (double)((1<<23)-adc_val[0]));
        temperature = 1 / (log(temp)/3435 + 1 / 298.15)-273.15;

        am_thermocouplie_t2v(type, temperature, &v1);

        am_zml166_adc_reg_set(handle,
                              AM_ZML166_ADC_ADC3_ADDR,
                              AM_ZML166_ADC_LVSHIFT_DISABLE|AM_ZML166_ADC_LVSCP_ENABLE);
        am_zml166_adc_mux_set(handle, AM_ZML166_ADC_INPS_AIN3 | AM_ZML166_ADC_INNS_AIN4);

        am_zml166_adc_gain_set(handle, 16);
        am_adc_read(adc_handle, 0, (uint32_t *)adc_val, AM_NELEMENTS(adc_val));
        temp  = (double)((double)adc_val[0] / 16.0 * 2500 / 8388607  );
        temp  = temp * p_para[8] + p_para[9];

        am_thermocouplie_v2t(type, temp + v1 * 1000, &temperature);

        temperature *= 1000;

        if(temperature < 0){
            temperature *= -1;
            am_kprintf("Tem = -%d.%03d��\r\n",
             (uint32_t)temperature/1000 ,
             (uint32_t)temperature%1000);
        }else{
            am_kprintf("Tem = %d.%03d��\r\n\r\n",
             (uint32_t)temperature/1000 ,
             (uint32_t)temperature%1000);
        }
        am_mdelay(300);
    }
}