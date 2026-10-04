class SRGP_WeaponDisplacementPersistence : JsonApiStruct
{
    bool ENABLE_PISTOL_SETTINGS = false;

    float ROTATION_X = 0;
    float ROTATION_Y = 0;
    float ROTATION_Z = 0;
    float OFFSET_X = 0;
    float OFFSET_Y = 0;
    float OFFSET_Z = 0;

    float PISTOL_ROTATION_X = 0;
    float PISTOL_ROTATION_Y = 0;
    float PISTOL_ROTATION_Z = 0;
    float PISTOL_OFFSET_X = 0;
    float PISTOL_OFFSET_Y = 0;
    float PISTOL_OFFSET_Z = 0;

    void SRGP_WeaponDisplacementPersistence()
    {
        RegV("ENABLE_PISTOL_SETTINGS");
        RegV("ROTATION_X");
        RegV("ROTATION_Y");
        RegV("ROTATION_Z");
        RegV("OFFSET_X");
        RegV("OFFSET_Y");
        RegV("OFFSET_Z");

        RegV("PISTOL_ROTATION_X");
        RegV("PISTOL_ROTATION_Y");
        RegV("PISTOL_ROTATION_Z");
        RegV("PISTOL_OFFSET_X");
        RegV("PISTOL_OFFSET_Y");
        RegV("PISTOL_OFFSET_Z");
    }

    static string GetFilePath()
    {
        return "$profile:SRGP_WeaponDisplacement/Settings.json";
    }

    static string GetPresetPath(int slot)
    {
        return string.Format("$profile:SRGP_WeaponDisplacement/Preset%1.json", slot);
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

        settings.Get("ENABLE_PISTOL_SETTINGS", ENABLE_PISTOL_SETTINGS);

        settings.Get("ROTATION_X", ROTATION_X);
        settings.Get("ROTATION_Y", ROTATION_Y);
        settings.Get("ROTATION_Z", ROTATION_Z);
        settings.Get("OFFSET_X", OFFSET_X);
        settings.Get("OFFSET_Y", OFFSET_Y);
        settings.Get("OFFSET_Z", OFFSET_Z);

        settings.Get("PISTOL_ROTATION_X", PISTOL_ROTATION_X);
        settings.Get("PISTOL_ROTATION_Y", PISTOL_ROTATION_Y);
        settings.Get("PISTOL_ROTATION_Z", PISTOL_ROTATION_Z);
        settings.Get("PISTOL_OFFSET_X", PISTOL_OFFSET_X);
        settings.Get("PISTOL_OFFSET_Y", PISTOL_OFFSET_Y);
        settings.Get("PISTOL_OFFSET_Z", PISTOL_OFFSET_Z);
    }

    void ApplyToGameSettings()
    {
        BaseContainer settings = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (!settings)
            return;

        settings.Set("ENABLE_PISTOL_SETTINGS", ENABLE_PISTOL_SETTINGS);

        settings.Set("ROTATION_X", ROTATION_X);
        settings.Set("ROTATION_Y", ROTATION_Y);
        settings.Set("ROTATION_Z", ROTATION_Z);
        settings.Set("OFFSET_X", OFFSET_X);
        settings.Set("OFFSET_Y", OFFSET_Y);
        settings.Set("OFFSET_Z", OFFSET_Z);

        settings.Set("PISTOL_ROTATION_X", PISTOL_ROTATION_X);
        settings.Set("PISTOL_ROTATION_Y", PISTOL_ROTATION_Y);
        settings.Set("PISTOL_ROTATION_Z", PISTOL_ROTATION_Z);
        settings.Set("PISTOL_OFFSET_X", PISTOL_OFFSET_X);
        settings.Set("PISTOL_OFFSET_Y", PISTOL_OFFSET_Y);
        settings.Set("PISTOL_OFFSET_Z", PISTOL_OFFSET_Z);
    }

    void SaveToFile()
    {
        EnsureDirectory();
        PackToFile(GetFilePath());
    }

    void SaveToSlot(int slot)
    {
        EnsureDirectory();
        PackToFile(GetPresetPath(slot));
    }

    bool LoadFromSlot(int slot)
    {
        string path = GetPresetPath(slot);
        if (!FileIO.FileExists(path))
            return false;

        LoadFromFile(path);
        return true;
    }
}