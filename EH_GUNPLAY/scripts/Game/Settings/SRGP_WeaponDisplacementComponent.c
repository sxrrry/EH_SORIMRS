class SRGP_WeaponDisplacementComponentClass : ScriptComponentClass
{
}

class SRGP_WeaponDisplacementComponent : ScriptComponent
{
    // onRplName указывает метод, который будет вызван на прокси-клиентах,
    // когда сервер пришлёт обновление этого свойства.
    [RplProp(onRplName: "OnSettingsUpdated")]
    float ROTATION_X = 0;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float ROTATION_Y = 0;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float ROTATION_Z = -12;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float OFFSET_X = -0.01;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float OFFSET_Y = 0;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float OFFSET_Z = 0;

    // Этот метод вызывается на сервере (Authority) для применения новых значений.
    // Клиентский RPC перенаправляется на сервер, поэтому тело выполняется там.
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_ApplyValues(float rotX, float rotY, float rotZ, float offX, float offY, float offZ)
    {
        if (!Replication.IsServer())
            return;

        ROTATION_X = rotX;
        ROTATION_Y = rotY;
        ROTATION_Z = rotZ;
        OFFSET_X   = offX;
        OFFSET_Y   = offY;
        OFFSET_Z   = offZ;

        // Уведомляем систему репликации, что RplProp-поля изменились.
        Replication.BumpMe();
    }

    // Этот метод вызывается на клиентах-прокси при получении обновления.
    // Здесь мы просто уведомляем модификатор о необходимости перечитать значения.
    void OnSettingsUpdated()
    {
        // Модификатор сам прочитает значения в OnCalculate,
        // так как его OnCalculate выполняется каждый кадр.
        // Если бы нам нужно было применить изменения немедленно, мы могли бы
        // найти модификатор и вызвать его метод здесь.
    }
}