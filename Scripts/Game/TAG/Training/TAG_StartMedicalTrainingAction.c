//------------------------------------------------------------------------------------------------
// Accion del instructor: "Iniciar practica de medicina"
// Va en Additional Actions del ActionsManagerComponent del NPC instructor.
//------------------------------------------------------------------------------------------------
class TAG_StartMedicalTrainingAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		TAG_MedicalTrainingManager mgr = TAG_MedicalTrainingManager.GetInstance();
		if (!mgr)
		{
			SetCannotPerformReason("No hay gestor de practicas");
			return false;
		}

		if (mgr.IsBusy())
		{
			SetCannotPerformReason("Ya hay una practica en curso");
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;

		TAG_MedicalTrainingManager mgr = TAG_MedicalTrainingManager.GetInstance();
		if (!mgr)
			return;

		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		mgr.StartTraining(playerId);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Iniciar practica de medicina";
		return true;
	}
}
