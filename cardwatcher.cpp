#include "cardwatcher.h"
#include <QTimer>

extern CK_FUNCTION_LIST_PTR pFunctionList;
extern CK_RV rv;

void CardWatcher::run()
{
    CK_SLOT_ID slot_ID = 0xFFFFFFFF;
    CK_SLOT_INFO slotInfo;
    while (TRUE)
    {
        rv = pFunctionList->C_WaitForSlotEvent(0,  // 0 для блокирования потока и CKF_DONT_BLOCK для неблокирования
                                               &slot_ID,  // Идентификатор слота, в котором произошло событие
                                               NULL_PTR);
        if (rv != CKR_OK)
            return;
        memset(&slotInfo, 0,  sizeof(CK_SLOT_INFO));

        /* Получить информацию о слоте */
        if (slot_ID == 0)
        {
            rv = pFunctionList->C_GetSlotInfo(slot_ID,
                                              &slotInfo);
            if (!(slotInfo.flags & CKF_TOKEN_PRESENT))
                emit Detached();
        }

        /* Получить информацию о слоте */
        if (slot_ID == 1)
        {
            rv = pFunctionList->C_GetSlotInfo(slot_ID,
                                              &slotInfo);
            if ((slotInfo.flags & CKF_TOKEN_PRESENT))
                emit Attached();
        }

    }

}




