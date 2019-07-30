#include "State.h"



/*******************************************************************************
** Function name:           W_WMStatus
** Descriptions:            д��״̬��
** input parameters:        Enum_Status_Name sta ��״̬��
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-9-21     
*******************************************************************************/
void W_State(uint32_t *status,uint32_t sta,bool enable)
{
  if( enable == true )
  {
    SET_1(*status,sta);
  }
  else
  {
    SET_0(*status,sta);
  }
}

/*******************************************************************************
** Function name:           Chack_WMStatus
** Descriptions:            ��״̬��ĳһ��״̬�Ƿ�Ϊ��
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-9-21     
*******************************************************************************/
bool R_State(uint32_t *status,uint32_t sta)
{
 return((bool)GET(*status,sta)); 
}
