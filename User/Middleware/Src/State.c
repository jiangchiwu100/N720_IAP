#include "State.h"



/*******************************************************************************
** Function name:           W_WMStatus
** Descriptions:            Ğ´±í×´Ì¬×Ö
** input parameters:        Enum_Status_Name sta ±í×´Ì¬×Ö
** output parameters:       void
** Returned value:          void 
** Created by:              ³ÌÎÀçô
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
** Descriptions:            ¶Á×´Ì¬×ÖÄ³Ò»¸ö×´Ì¬ÊÇ·ñÎªÕæ
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ³ÌÎÀçô
** Created Date:            2016-9-21     
*******************************************************************************/
bool R_State(uint32_t *status,uint32_t sta)
{
 return((bool)GET(*status,sta)); 
}
