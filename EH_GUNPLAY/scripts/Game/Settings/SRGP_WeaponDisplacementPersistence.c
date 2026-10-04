class SRGP_WeaponDisplacementPersistence : JsonApiStruct
{
    float ROTATION_X = 0;
    float ROTATION_Y = 0;
    float ROTATION_Z = 0;
    float OFFSET_X = 0;
    float OFFSET_Y = 0;
    float OFFSET_Z = 0;

    void SRGP_WeaponDisplacementPersistence()
    {
        RegV("ROTATION_X");
        RegV("ROTATION_Y");
        RegV("ROTATION_Z");
        RegV("OFFSET_X");
        RegV("OFFSET_Y");
        RegV("OFFSET_Z");
    }

    static string GetFilePath()
    {
        return "$profile:SRGP_WeaponDisplacement/Settings.json";
    }

    static void EnsureDirectory()
    {
        FileIO.MakeDirectory("$profile:SRGP_WeaponDisplacement");
    }

    void LoadFromFileOrDefaults()
    {
        EnsureDirectory();

        if (FileIO.FileExists(GetFilePath()))
        {
            LoadFromFile(GetFilePath());
            return;
        }

        LoadFromGameSettings();
        SaveToFile();
    }

    void LoadFromGameSettings()
    {
        BaseContainer settings = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (!settings)
            return;

        settings.Get("ROTATION_X", ROTATION_X);
        settings.Get("ROTATION_Y", ROTATION_Y);
        settings.Get("ROTATION_Z", ROTATION_Z);
        settings.Get("OFFSET_X", OFFSET_X);
        settings.Get("OFFSET_Y", OFFSET_Y);
        settings.Get("OFFSET_Z", OFFSET_Z);
    }

    void ApplyToGameSettings()
    {
        BaseContainer settings = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (!settings)
            return;

        settings.Set("ROTATION_X", ROTATION_X);
        settings.Set("ROTATION_Y", ROTATION_Y);
        settings.Set("ROTATION_Z", ROTATION_Z);
        settings.Set("OFFSET_X", OFFSET_X);
        settings.Set("OFFSET_Y", OFFSET_Y);
        settings.Set("OFFSET_Z", OFFSET_Z);
    }

    void SaveToFile()
    {
        EnsureDirectory();
        PackToFile(GetFilePath());
    }
}