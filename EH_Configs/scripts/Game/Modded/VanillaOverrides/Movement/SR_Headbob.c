modded class SCR_CharacterCameraHandlerComponent
{
	override private void UpdateViewBob(float pDt)
	{
		pDt *= 2;
		super.UpdateViewBob(pDt);
	}

}