//------------------------------------------------------------------------------------------------
// Accion opcional del instructor: "Cancelar practica de medicina"
// Solo aparece si hay una practica en curso.
//------------------------------------------------------------------------------------------------
class TAG_CancelMedicalTrainingAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		TAG_MedicalTrainingManager mgr = TAG_MedicalTrainingManager.GetInstance();
		return mgr && mgr.IsBusy();
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;

		TAG_MedicalTrainingManager mgr = TAG_MedicalTrainingManager.GetInstance();
		if (mgr)
			mgr.CancelTraining();
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Cancelar practica de medicina";
		return true;
	}
}
