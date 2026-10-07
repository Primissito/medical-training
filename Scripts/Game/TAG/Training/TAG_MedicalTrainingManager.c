//------------------------------------------------------------------------------------------------
// Practicas de medicina - gestor (version ACE Medical completa)
//
// Dependencias del mod (.gproj):
//   ACE Core, ACE Medical Core, ACE Medical Hitzones, ACE Medical Breathing, ACE Medical Circulation
//
// La entidad necesita un RplComponent para replicar m_bBusy a los clientes.
// Todo el codigo de la practica se ejecuta solo en el servidor.
//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "TAG/Training", description: "Gestor de practicas de medicina (ACE Medical)")]
class TAG_MedicalTrainingManagerClass : GenericEntityClass
{
}

class TAG_MedicalTrainingManager : GenericEntity
{
	//=== Herido ===================================================================================
	[Attribute("", UIWidgets.ResourceNamePicker, "Prefab del herido (personaje SIN grupo de IA)", "et", category: "Herido")]
	protected ResourceName m_sPatientPrefab;

	[Attribute("MedTraining_Spawn", desc: "Nombre de la entidad donde aparece el herido", category: "Herido")]
	protected string m_sSpawnPointName;

	[Attribute("0.5", desc: "Sangre inicial minima (0-1)", category: "Herido")]
	protected float m_fStartBloodMin;

	[Attribute("0.8", desc: "Sangre inicial maxima (0-1)", category: "Herido")]
	protected float m_fStartBloodMax;

	[Attribute("0.3", desc: "Salud de la zona de dolor ACE al empezar (0 = dolor maximo, 1 = sin dolor)", category: "Herido")]
	protected float m_fStartPainHealth;

	[Attribute("1", desc: "Empieza inconsciente", category: "Herido")]
	protected bool m_bStartUnconscious;

	//=== Escenarios ===============================================================================
	[Attribute("1", desc: "Escenarios maximos combinados en un mismo herido (1 = solo uno)", category: "Escenarios")]
	protected int m_iMaxScenarios;

	[Attribute("1", desc: "Sangrados en extremidades/torso", category: "Escenarios")]
	protected bool m_bScenarioBleeding;

	[Attribute("1", desc: "Arteria femoral rota (sangrado masivo)", category: "Escenarios")]
	protected bool m_bScenarioFemoral;

	[Attribute("1", desc: "Neumotorax simple (chest seal)", category: "Escenarios")]
	protected bool m_bScenarioPneumothorax;

	[Attribute("1", desc: "Neumotorax a tension (descompresion con kit NCD)", category: "Escenarios")]
	protected bool m_bScenarioTensionPneumothorax;

	[Attribute("1", desc: "Via aerea obstruida o con vomito", category: "Escenarios")]
	protected bool m_bScenarioAirway;

	[Attribute("1", desc: "Parada cardiaca (RCP + epinefrina)", category: "Escenarios")]
	protected bool m_bScenarioCardiacArrest;

	[Attribute("", desc: "Zonas para sangrados. Vacio = zonas por defecto de ACE (brazos, piernas, torso)", category: "Escenarios")]
	protected ref array<string> m_aHitZones;

	[Attribute("1", desc: "Sangrados minimos (escenario de sangrado)", category: "Escenarios")]
	protected int m_iMinBleedings;

	[Attribute("3", desc: "Sangrados maximos (escenario de sangrado)", category: "Escenarios")]
	protected int m_iMaxBleedings;

	[Attribute("0.1", desc: "Salud que se deja a la arteria femoral (0-1, mas bajo = peor)", category: "Escenarios")]
	protected float m_fFemoralHealth;

	[Attribute("0.36", desc: "Volumen pulmonar perdido al empezar un neumotorax (0-0.75)", category: "Escenarios")]
	protected float m_fPneumothoraxStartScale;

	[Attribute("1", desc: "Permitir parada cardiaca en la IA mientras dura la practica (si no, ACE mata al herido al entrar en parada)", category: "Escenarios")]
	protected bool m_bForceAICardiacArrest;

	//=== Evaluacion ===============================================================================
	[Attribute("420", desc: "Tiempo limite en segundos", category: "Evaluacion")]
	protected float m_fTimeLimit;

	[Attribute("10", desc: "Segundos seguidos que el herido debe estar estable para aprobar", category: "Evaluacion")]
	protected int m_iStableSecondsRequired;

	[Attribute("0.6", desc: "Sangre minima (0-1)", category: "Evaluacion")]
	protected float m_fMinBlood;

	[Attribute("1", desc: "Exigir que no tenga dolor (morfina)", category: "Evaluacion")]
	protected bool m_bRequireNoPain;

	[Attribute("1", desc: "Exigir neumotorax resuelto (chest seal puesto)", category: "Evaluacion")]
	protected bool m_bRequireChestSeal;

	[Attribute("90", desc: "SpO2 minima (%)", category: "Evaluacion")]
	protected float m_fMinSpO2;

	[Attribute("40", desc: "Pulso minimo (lpm)", category: "Evaluacion")]
	protected float m_fMinHeartRate;

	[Attribute("140", desc: "Pulso maximo (lpm)", category: "Evaluacion")]
	protected float m_fMaxHeartRate;

	[Attribute("1", desc: "Aceptar estado vital UNSTABLE como aprobado", category: "Evaluacion")]
	protected bool m_bAcceptUnstable;

	[Attribute("0", desc: "Aceptar estado vital CRITICAL como aprobado", category: "Evaluacion")]
	protected bool m_bAcceptCritical;

	[Attribute("0", desc: "Suspender si el herido entra en Second Chance de ACE", category: "Evaluacion")]
	protected bool m_bFailOnSecondChance;

	[Attribute("30", desc: "Segundos hasta borrar al herido y la tarea al terminar", category: "Evaluacion")]
	protected int m_iCleanupDelay;

	//=== Tarea nativa =============================================================================
	[Attribute("", UIWidgets.ResourceNamePicker, "Prefab de tarea (SCR_Task) que se crea para el alumno", "et", category: "Tarea")]
	protected ResourceName m_sTaskPrefab;

	[Attribute("Practica de medicina", desc: "Nombre de la tarea", category: "Tarea")]
	protected string m_sTaskName;

	[Attribute("Evalua al herido, identifica sus lesiones y estabilizalo antes de que se acabe el tiempo.", desc: "Descripcion de la tarea", category: "Tarea")]
	protected string m_sTaskDesc;

	//=== Estado ===================================================================================
	[RplProp()]
	protected bool m_bBusy;

	protected static TAG_MedicalTrainingManager s_Instance;
	protected IEntity m_Patient;
	protected SCR_Task m_Task;
	protected ref array<TAG_EMedScenario> m_aActiveScenarios = {};
	protected int m_iTraineeId = -1;
	protected float m_fElapsed;
	protected bool m_bActive;
	protected int m_iRunCounter;
	protected int m_iStableSeconds;

	// Informe
	protected bool m_bHadCardiacArrest;
	protected float m_fLowestSpO2 = 100;

	// Ajuste temporal de ACE
	protected bool m_bAIArrestOverridden;
	protected bool m_bPrevAIArrest;

	//------------------------------------------------------------------------------------------------
	void TAG_MedicalTrainingManager(IEntitySource src, IEntity parent)
	{
		s_Instance = this;
	}

	//------------------------------------------------------------------------------------------------
	static TAG_MedicalTrainingManager GetInstance()
	{
		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------
	//! Replicado: lo usa la accion en el cliente para bloquear el boton
	bool IsBusy()
	{
		return m_bBusy;
	}

	//------------------------------------------------------------------------------------------------
	protected void SetBusy(bool busy)
	{
		m_bBusy = busy;
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	protected SCR_CharacterDamageManagerComponent GetPatientDamage()
	{
		if (!m_Patient)
			return null;

		return SCR_CharacterDamageManagerComponent.Cast(m_Patient.FindComponent(SCR_CharacterDamageManagerComponent));
	}

	//------------------------------------------------------------------------------------------------
	protected ACE_Medical_VitalsComponent GetPatientVitals()
	{
		if (!m_Patient)
			return null;

		return ACE_Medical_VitalsComponent.Cast(m_Patient.FindComponent(ACE_Medical_VitalsComponent));
	}

	//------------------------------------------------------------------------------------------------
	//! ACE mata a la IA al entrar en parada salvo que m_bCardiacArrestForAIEnabled este activo.
	//! Lo activamos solo mientras dura la practica y lo restauramos al terminar.
	protected void EnableAICardiacArrest()
	{
		if (!m_bForceAICardiacArrest || m_bAIArrestOverridden)
			return;

		ACE_Medical_Circulation_Settings settings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
		if (!settings)
			return;

		m_bPrevAIArrest = settings.m_bCardiacArrestForAIEnabled;
		settings.m_bCardiacArrestForAIEnabled = true;
		m_bAIArrestOverridden = true;
	}

	//------------------------------------------------------------------------------------------------
	protected void RestoreAICardiacArrest()
	{
		if (!m_bAIArrestOverridden)
			return;

		ACE_Medical_Circulation_Settings settings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
		if (settings)
			settings.m_bCardiacArrestForAIEnabled = m_bPrevAIArrest;

		m_bAIArrestOverridden = false;
	}

	//------------------------------------------------------------------------------------------------
	protected void CreateTask(int playerId, vector position)
	{
		if (m_sTaskPrefab.IsEmpty())
			return;

		SCR_TaskSystem taskSystem = SCR_TaskSystem.GetInstance();
		if (!taskSystem)
			return;

		m_iRunCounter++;
		string taskId = string.Format("TAG_MedTraining_%1", m_iRunCounter);
		m_Task = taskSystem.CreateTask(m_sTaskPrefab, taskId, m_sTaskName, m_sTaskDesc, position, playerId);
		if (!m_Task)
			return;

		SCR_TaskExecutorPlayer executor = new SCR_TaskExecutorPlayer();
		executor.SetPlayerID(playerId);
		taskSystem.AssignTask(m_Task, executor, true);
	}

	//------------------------------------------------------------------------------------------------
	protected void SetTaskResult(bool success)
	{
		SCR_TaskSystem taskSystem = SCR_TaskSystem.GetInstance();
		if (!taskSystem || !m_Task)
			return;

		if (success)
			taskSystem.SetTaskState(m_Task, SCR_ETaskState.COMPLETED);
		else
			taskSystem.SetTaskState(m_Task, SCR_ETaskState.FAILED);
	}

	//------------------------------------------------------------------------------------------------
	protected void Cleanup()
	{
		SCR_TaskSystem taskSystem = SCR_TaskSystem.GetInstance();
		if (taskSystem && m_Task)
			taskSystem.DeleteTask(m_Task);
		m_Task = null;

		if (m_Patient)
			SCR_EntityHelper.DeleteEntityAndChildren(m_Patient);
		m_Patient = null;

		RestoreAICardiacArrest();

		m_iTraineeId = -1;
		m_aActiveScenarios.Clear();
		SetBusy(false);

		Print("[MedTraining] Practica limpiada, lista para una nueva", LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	protected string GetScenarioList()
	{
		string result;
		foreach (int i, TAG_EMedScenario scenario : m_aActiveScenarios)
		{
			if (i > 0)
				result += ", ";
			result += typename.EnumToString(TAG_EMedScenario, scenario);
		}
		return result;
	}

	//------------------------------------------------------------------------------------------------
	//! immediate = true borra al herido y la tarea al momento (cancelar)
	protected void Finish(bool success, string reason, bool immediate = false)
	{
		// CheckPatient y ApplyAirway se paran solos al ver m_bActive = false
		// (Enforce no permite referenciar metodos definidos mas abajo)
		m_bActive = false;

		SetTaskResult(success);

		Print(string.Format("[MedTraining] Jugador %1 | aprobado: %2 | %3 | %4 s | escenarios: %5 | parada: %6 | SpO2 minima: %7",
			m_iTraineeId, success, reason, m_fElapsed, GetScenarioList(), m_bHadCardiacArrest, m_fLowestSpO2), LogLevel.NORMAL);

		// Por si quedaba una limpieza pendiente
		GetGame().GetCallqueue().Remove(Cleanup);

		if (immediate || m_iCleanupDelay <= 0)
			Cleanup();
		else
			GetGame().GetCallqueue().CallLater(Cleanup, m_iCleanupDelay * 1000);
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsVitalStateAccepted(ACE_Medical_EVitalStateID state)
	{
		if (state == ACE_Medical_EVitalStateID.STABLE)
			return true;

		if (state == ACE_Medical_EVitalStateID.UNSTABLE)
			return m_bAcceptUnstable;

		if (state == ACE_Medical_EVitalStateID.CRITICAL)
			return m_bAcceptCritical;

		// RESUSCITATION y CARDIAC_ARREST nunca aprueban
		return false;
	}

	//------------------------------------------------------------------------------------------------
	//! Todas las condiciones para considerar al herido estabilizado
	protected bool IsPatientStable(SCR_CharacterDamageManagerComponent dmg, ACE_Medical_VitalsComponent vitals)
	{
		// Sangrado y sangre (ACE Medical Core / vanilla)
		if (dmg.IsBleeding())
			return false;

		HitZone bloodHZ = dmg.GetBloodHitZone();
		if (bloodHZ && bloodHZ.GetHealthScaled() < m_fMinBlood)
			return false;

		// Dolor (ACE Medical Core)
		if (m_bRequireNoPain && dmg.ACE_Medical_IsInPain())
			return false;

		if (!vitals)
			return true;

		// Respiracion (ACE Medical Breathing)
		if (!vitals.CanBreath())
			return false;

		if (m_bRequireChestSeal && vitals.GetPneumothoraxScale() > 0)
			return false;

		if (vitals.GetSpO2() < m_fMinSpO2)
			return false;

		// Circulacion (ACE Medical Circulation)
		if (!IsVitalStateAccepted(vitals.GetVitalStateID()))
			return false;

		float heartRate = vitals.GetHeartRate();
		if (heartRate < m_fMinHeartRate || heartRate > m_fMaxHeartRate)
			return false;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Se ejecuta cada segundo en el servidor
	protected void CheckPatient()
	{
		if (!m_bActive)
		{
			GetGame().GetCallqueue().Remove(CheckPatient);
			return;
		}

		m_fElapsed += 1;

		SCR_CharacterDamageManagerComponent dmg = GetPatientDamage();
		if (!dmg || dmg.IsDestroyed())
		{
			Finish(false, "el herido ha muerto");
			return;
		}

		ACE_Medical_VitalsComponent vitals = GetPatientVitals();

		// Datos para el informe
		if (vitals)
		{
			if (vitals.GetVitalStateID() == ACE_Medical_EVitalStateID.CARDIAC_ARREST)
				m_bHadCardiacArrest = true;

			float spo2 = vitals.GetSpO2();
			if (spo2 < m_fLowestSpO2)
				m_fLowestSpO2 = spo2;
		}

		if (m_bFailOnSecondChance && dmg.ACE_Medical_WasSecondChanceGranted())
		{
			Finish(false, "el herido ha estado a punto de morir (Second Chance)");
			return;
		}

		if (m_fElapsed >= m_fTimeLimit)
		{
			Finish(false, "tiempo agotado");
			return;
		}

		if (!IsPatientStable(dmg, vitals))
		{
			m_iStableSeconds = 0;
			return;
		}

		m_iStableSeconds++;
		if (m_iStableSeconds >= m_iStableSecondsRequired)
			Finish(true, "herido estabilizado");
	}

	//------------------------------------------------------------------------------------------------
	protected void ApplyBleeding(SCR_CharacterDamageManagerComponent dmg)
	{
		array<string> pool = {};
		pool.Copy(m_aHitZones);

		int count = Math.RandomIntInclusive(m_iMinBleedings, m_iMaxBleedings);
		for (int i = 0; i < count && pool.Count() > 0; i++)
		{
			int idx = Math.RandomInt(0, pool.Count());
			dmg.AddParticularBleeding(pool[idx]);
			pool.Remove(idx);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Danar la arteria femoral hace que ACE Medical Hitzones cree el sangrado masivo y deje KO al herido
	protected void ApplyFemoral(SCR_CharacterDamageManagerComponent dmg)
	{
		string hitZoneName = "ACE_Medical_LFemoralArtery";
		if (Math.RandomInt(0, 2) == 1)
			hitZoneName = "ACE_Medical_RFemoralArtery";

		HitZone artery = dmg.GetHitZoneByName(hitZoneName);
		if (artery)
			artery.SetHealthScaled(m_fFemoralHealth);
		else
			Print("[MedTraining] No existe la zona " + hitZoneName + ". Esta cargado ACE Medical Hitzones?", LogLevel.WARNING);
	}

	//------------------------------------------------------------------------------------------------
	//! Se aplica con retraso: ACE recalcula la obstruccion cuando el herido cambia de postura al caer
	protected void ApplyAirway()
	{
		ACE_Medical_VitalsComponent vitals = GetPatientVitals();
		if (!vitals || !m_bActive)
			return;

		// Mitad de las veces lengua (inclinar la cabeza / canula), mitad vomito (limpiar via aerea)
		if (Math.RandomInt(0, 2) == 0)
			vitals.SetIsAirwayObstructed(true);
		else
			vitals.SetIsAirwayOccluded(true);
	}

	//------------------------------------------------------------------------------------------------
	protected void ApplyScenario(TAG_EMedScenario scenario, SCR_CharacterDamageManagerComponent dmg, ACE_Medical_VitalsComponent vitals)
	{
		switch (scenario)
		{
			case TAG_EMedScenario.BLEEDING:
			{
				ApplyBleeding(dmg);
				break;
			}
			case TAG_EMedScenario.FEMORAL:
			{
				ApplyFemoral(dmg);
				break;
			}
			case TAG_EMedScenario.PNEUMOTHORAX:
			{
				if (vitals)
					vitals.SetPneumothoraxScale(m_fPneumothoraxStartScale);
				break;
			}
			case TAG_EMedScenario.TENSION_PNEUMOTHORAX:
			{
				if (vitals)
				{
					vitals.SetPneumothoraxScale(m_fPneumothoraxStartScale);
					vitals.SetHasTensionPneumothorax(true);
				}
				break;
			}
			case TAG_EMedScenario.AIRWAY:
			{
				GetGame().GetCallqueue().CallLater(ApplyAirway, 3000);
				break;
			}
			case TAG_EMedScenario.CARDIAC_ARREST:
			{
				if (vitals)
					vitals.SetVitalStateID(ACE_Medical_EVitalStateID.CARDIAC_ARREST);
				break;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void PickScenarios()
	{
		array<TAG_EMedScenario> pool = {};
		if (m_bScenarioBleeding)
			pool.Insert(TAG_EMedScenario.BLEEDING);
		if (m_bScenarioFemoral)
			pool.Insert(TAG_EMedScenario.FEMORAL);
		if (m_bScenarioPneumothorax)
			pool.Insert(TAG_EMedScenario.PNEUMOTHORAX);
		if (m_bScenarioTensionPneumothorax)
			pool.Insert(TAG_EMedScenario.TENSION_PNEUMOTHORAX);
		if (m_bScenarioAirway)
			pool.Insert(TAG_EMedScenario.AIRWAY);
		if (m_bScenarioCardiacArrest)
			pool.Insert(TAG_EMedScenario.CARDIAC_ARREST);

		if (pool.IsEmpty())
			pool.Insert(TAG_EMedScenario.BLEEDING);

		m_aActiveScenarios.Clear();
		int maxScenarios = m_iMaxScenarios;
		if (maxScenarios < 1)
			maxScenarios = 1;
		int count = Math.RandomIntInclusive(1, maxScenarios);
		for (int i = 0; i < count && pool.Count() > 0; i++)
		{
			int idx = Math.RandomInt(0, pool.Count());
			m_aActiveScenarios.Insert(pool[idx]);
			pool.Remove(idx);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void ApplyInjuries()
	{
		SCR_CharacterDamageManagerComponent dmg = GetPatientDamage();
		if (!dmg)
		{
			Finish(false, "no se encontro el damage manager del herido");
			return;
		}

		ACE_Medical_VitalsComponent vitals = GetPatientVitals();
		if (!vitals)
			Print("[MedTraining] El herido no tiene ACE_Medical_VitalsComponent. Estan cargados Breathing y Circulation?", LogLevel.WARNING);

		// Base comun: sangre perdida y dolor
		HitZone bloodHZ = dmg.GetBloodHitZone();
		if (bloodHZ)
			bloodHZ.SetHealthScaled(Math.RandomFloat(m_fStartBloodMin, m_fStartBloodMax));

		HitZone painHZ = dmg.ACE_Medical_GetPainHitZone();
		if (painHZ)
			painHZ.SetHealthScaled(m_fStartPainHealth);

		// Inconsciencia: igual que hace ACE, vaciando la resiliencia
		if (m_bStartUnconscious)
		{
			HitZone resilienceHZ = dmg.GetResilienceHitZone();
			if (resilienceHZ)
				resilienceHZ.SetHealthScaled(0);
			dmg.ForceUnconsciousness();
		}

		foreach (TAG_EMedScenario scenario : m_aActiveScenarios)
		{
			ApplyScenario(scenario, dmg, vitals);
		}

		GetGame().GetCallqueue().CallLater(CheckPatient, 1000, true);
	}

	//------------------------------------------------------------------------------------------------
	//! Llamado por TAG_StartMedicalTrainingAction. Solo en servidor.
	bool StartTraining(int playerId)
	{
		if (!Replication.IsServer() || m_bBusy)
			return false;

		// Zonas de ACE Medical Hitzones (DamageManager_Character_Base.ct)
		if (!m_aHitZones)
			m_aHitZones = {};
		if (m_aHitZones.IsEmpty())
			m_aHitZones = {"LArm", "RArm", "LForearm", "RForearm", "LThigh", "RThigh", "LCalf", "RCalf", "Chest", "Abdomen"};

		IEntity spawnPoint = GetGame().GetWorld().FindEntityByName(m_sSpawnPointName);
		if (!spawnPoint)
		{
			Print("[MedTraining] No existe el punto " + m_sSpawnPointName, LogLevel.ERROR);
			return false;
		}

		// Antes de crear al herido, para que una parada no lo mate
		EnableAICardiacArrest();

		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		spawnPoint.GetWorldTransform(params.Transform);

		m_Patient = GetGame().SpawnEntityPrefab(Resource.Load(m_sPatientPrefab), GetWorld(), params);
		if (!m_Patient)
		{
			Print("[MedTraining] No se pudo crear el herido", LogLevel.ERROR);
			RestoreAICardiacArrest();
			return false;
		}

		m_iTraineeId = playerId;
		m_fElapsed = 0;
		m_iStableSeconds = 0;
		m_bHadCardiacArrest = false;
		m_fLowestSpO2 = 100;
		m_bActive = true;
		SetBusy(true);

		PickScenarios();
		CreateTask(playerId, spawnPoint.GetOrigin());

		// Medio segundo para que el personaje y ACE terminen de inicializarse
		GetGame().GetCallqueue().CallLater(ApplyInjuries, 500);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Para la accion de cancelar o para admins
	void CancelTraining()
	{
		if (!Replication.IsServer() || !m_bBusy)
			return;

		// Activa: se cierra la tarea y se limpia al momento
		if (m_bActive)
		{
			Finish(false, "cancelada", true);
			return;
		}

		// Ya terminada pero esperando la limpieza de 30 s: limpiar ya
		GetGame().GetCallqueue().Remove(Cleanup);
		Cleanup();
	}

	//------------------------------------------------------------------------------------------------
	void ~TAG_MedicalTrainingManager()
	{
		if (GetGame() && GetGame().GetCallqueue())
		{
			GetGame().GetCallqueue().Remove(CheckPatient);
			GetGame().GetCallqueue().Remove(ApplyInjuries);
			GetGame().GetCallqueue().Remove(ApplyAirway);
			GetGame().GetCallqueue().Remove(Cleanup);
		}

		RestoreAICardiacArrest();

		if (s_Instance == this)
			s_Instance = null;
	}
}
