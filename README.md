# Medical Training — Prácticas de medicina con ACE Medical

Mod de Arma Reforger para servidores milsim: un instructor médico genera un herido de IA con lesiones aleatorias y el jugador tiene que diagnosticarlo y estabilizarlo con el material de **ACE Medical** antes de que se acabe el tiempo. El resultado aparece como tarea nativa (completada o fallida) y se registra en la consola del servidor.

> **Estado:** en pruebas. El código usa la API de las ramas de desarrollo de [ACE-Anvil](https://github.com/acemod/ACE-Anvil) (`medical/add-breathing` y `medical/add-circulation`). Si vuestra versión de ACE del Workshop cambia algún nombre, saldrán errores de compilación en `ACE_Medical_*`.

## Escenarios

| Escenario | Mod de ACE | Qué tiene el herido | Qué tiene que hacer el alumno |
| --- | --- | --- | --- |
| Sangrado | Medical Core | 1–3 sangrados en brazos, piernas o torso | Vendar (el torniquete solo gana tiempo) |
| Arteria femoral | Medical Hitzones | Sangrado masivo y KO | Torniquete y vendaje |
| Neumotórax | Medical Breathing | Pérdida de volumen pulmonar que empeora cada minuto | Chest seal |
| Neumotórax a tensión | Medical Breathing | No puede respirar | Descompresión con kit NCD y chest seal |
| Vía aérea | Medical Breathing | Lengua caída o vómito | Inclinar la cabeza o cánula / limpiar la vía aérea |
| Parada cardiaca | Medical Circulation | Estado vital en parada | RCP y epinefrina |

Además, el herido siempre empieza con menos sangre, con dolor y (por defecto) inconsciente.

**Para aprobar**, el herido debe cumplir todo esto durante 10 s seguidos: sin sangrados, sangre ≥ 60 %, sin dolor, respira, sin neumotórax, SpO2 ≥ 90 %, pulso entre 40 y 140 lpm y estado vital `STABLE` (o `UNSTABLE`, configurable). **Suspende** si muere o pasan 7 minutos. Todos los valores se ajustan en el gestor.

## Requisitos

- Arma Reforger Tools (Enfusion Workbench)
- Dependencias del mod:
  - ACE Core
  - ACE Medical Core
  - ACE Medical Hitzones
  - ACE Medical Breathing
  - ACE Medical Circulation

## Contenido del repositorio

```
Scripts/Game/TAG/Training/
├── TAG_EMedScenario.c                  Enum de escenarios
├── TAG_MedicalTrainingManager.c        Gestor: crea el herido, aplica escenarios, evalúa, gestiona la tarea
├── TAG_StartMedicalTrainingAction.c    Acción del instructor "Iniciar práctica de medicina"
└── TAG_CancelMedicalTrainingAction.c   Acción opcional "Cancelar práctica de medicina"
```

Los prefabs (herido, instructor, gestor) y el mundo no van en el repositorio: se crean en Workbench siguiendo la guía de abajo, porque dependen de vuestro mapa.

## Setup

### 1. Crear o abrir el proyecto

1. Abre **Arma Reforger Tools** y crea un proyecto nuevo (*Add Project → Create New*) o abre el de vuestro servidor.
2. Clona este repositorio **dentro** de la carpeta del proyecto, o copia la carpeta `Scripts/` en ella. La ruta final debe ser:
   ```
   <TuProyecto>/Scripts/Game/TAG/Training/TAG_MedicalTrainingManager.c
   ```
   Si los scripts no están bajo `Scripts/Game/`, Workbench no los compila y el gestor no aparece en ninguna lista.
3. Si usáis otro prefijo en lugar de `TAG_`, renombra clases y archivos en los cuatro scripts.

### 2. Dependencias

1. *Workbench → Options → Game Project → Dependencies → Add*.
2. Añade ACE Core, ACE Medical Core, ACE Medical Hitzones, ACE Medical Breathing y ACE Medical Circulation, en las mismas versiones que el servidor.

### 3. Compilar

1. *Editors → Script Editor*.
2. **F7** (*Build → Compile and Reload Scripts*).
3. La consola debe terminar sin errores en rojo. Si sale `Undefined function` o `Unknown type` en algo `ACE_Medical_*`, vuestra versión de ACE no coincide con la usada: abrid un issue con el error.
4. **Reinicia Workbench** después de la primera compilación: el World Editor no siempre muestra las entidades de script nuevas hasta reiniciar.

### 4. Prefab del herido

1. En el Resource Browser, duplica un personaje civil o soldado base (clic derecho → *Duplicate*) como `MedTraining_Patient.et`.
2. Quítale la IA (`AIControlComponent` / agente de IA), el arma y el equipo de combate.
3. Comprueba que tiene `ACE_Medical_VitalsComponent`. Si no, solo funcionarán los escenarios de sangrado (la consola avisa).

### 5. Prefab de la tarea

En `Prefabs/Tasks/` elige un `SCR_Task` básico (sin trigger) y copia su ResourceName (clic derecho → *Copy Resource Name*).

### 6. Preparar el mapa

1. Monta la zona de entrenamiento (tienda médica, camilla).
2. Crea una entidad vacía (`GenericEntity`) con *Name* exactamente `MedTraining_Spawn`. El herido aparece en su posición y con su orientación.
3. Pon al lado un arsenal con material de ACE: vendas, torniquetes, morfina, epinefrina, salino, chest seals, kits NCD y cánulas.

### 7. Colocar el gestor

**Opción A — desde el World Editor**

1. En la lista de entidades (pestaña *Create* / *Entities*) busca `MedicalTraining` (todo junto, sin espacios); está en **TAG → Training**.
2. Arrastra `TAG_MedicalTrainingManager` al mapa (es invisible; la posición da igual).
3. Para guardarlo como prefab, arrástralo desde el **Hierarchy** a una carpeta del **Resource Browser** y renómbralo `MedTraining_Manager.et`.

**Opción B — si no aparece en la lista**

1. Crea a mano el archivo `Prefabs/Training/MedTraining_Manager.et` con este contenido:
   ```
   TAG_MedicalTrainingManager {
   }
   ```
2. En Workbench, *Refresh* en la carpeta, doble clic para abrirlo, configúralo (pasos de abajo) y guarda.
3. Arrástralo al mapa.

**En ambos casos:**

1. Añade un **`RplComponent`** (*Add Component*). Sin él, el botón del instructor no se bloquea para el resto mientras hay una práctica.
2. Rellena los campos en *Object Properties*:

| Categoría | Campo | Valor |
| --- | --- | --- |
| Herido | Patient Prefab | ResourceName de `MedTraining_Patient.et` |
| Herido | Spawn Point Name | `MedTraining_Spawn` |
| Escenarios | Max Scenarios | 1 (2–3 para politrauma) |
| Escenarios | Scenario Bleeding … Cardiac Arrest | Activados los que queráis |
| Escenarios | Hit Zones | **Vacío** (usa las zonas de ACE por defecto) |
| Escenarios | Force AI Cardiac Arrest | Activado |
| Evaluacion | Time Limit | 420 |
| Evaluacion | Cleanup Delay | 30 (0 = borrar al momento) |
| Tarea | Task Prefab | ResourceName del paso 5 |

3. Coloca **un solo gestor** por mapa y guarda el mundo (**Ctrl+S**).

### 8. Instructor

1. Duplica un NPC (o el de otra actividad) y abre su `ActionsManagerComponent`.
2. En **Action Contexts** añade uno (`MedTrainingContext`) con su `UIInfo` y su `Position` (PointInfo a la altura del pecho).
3. En **Additional Actions** añade `TAG_StartMedicalTrainingAction` y, si quieres, `TAG_CancelMedicalTrainingAction`. **Cada acción necesita su propio `UIInfo`** y el contexto asignado; si no, no aparecen.
4. Colócalo junto a la zona de entrenamiento.

### 9. Probar

1. Pon *Max Scenarios* a 1 y activa un solo escenario cada vez.
2. Play → habla con el instructor → trata al herido.
3. Al terminar, la consola muestra una línea como:
   ```
   [MedTraining] Jugador 1 | aprobado: 1 | herido estabilizado | 143 s | escenarios: PNEUMOTHORAX | parada: 0 | SpO2 minima: 88.4
   ```
4. Repite en un servidor dedicado local con dos clientes.

## Mensajes de consola

| Mensaje | Causa |
| --- | --- |
| `No existe el punto MedTraining_Spawn` | El *Name* de la entidad del paso 6 no coincide |
| `No se pudo crear el herido` | *Patient Prefab* vacío o mal pegado |
| `El herido no tiene ACE_Medical_VitalsComponent` | Faltan Breathing/Circulation o el prefab del herido no hereda de un personaje con ACE |
| `No existe la zona ACE_Medical_LFemoralArtery` | Falta ACE Medical Hitzones |
| `Practica limpiada, lista para una nueva` | Se borraron el herido y la tarea; se puede iniciar otra |

## Comportamiento a tener en cuenta

- **Una práctica a la vez** en todo el servidor.
- **Cancelar** borra al herido y la tarea al momento. Al aprobar o suspender se espera *Cleanup Delay* segundos (30 por defecto) antes de poder iniciar otra.
- **Parada cardiaca en IA:** ACE mata a la IA que entra en parada salvo que su ajuste `m_bCardiacArrestForAIEnabled` esté activo. El gestor lo activa solo durante la práctica y lo restaura al terminar; mientras tanto, cualquier otra IA del servidor que entre en parada tampoco muere. Si no os conviene, desactivad *Force AI Cardiac Arrest* y el escenario de parada.
- El herido **no dice qué tiene**: el alumno tiene que diagnosticarlo.

## Pendiente de verificar

- Que bajar la salud de la arteria femoral por script dispare el sangrado de ACE igual que un disparo.
- Que forzar `CARDIAC_ARREST` en un herido recién creado no lo saque ACE de la parada al instante.

## Ideas futuras

- Varios heridos a la vez para triaje.
- Evacuación: aprobar al llevar al herido hasta un punto.
- Informe en pantalla para el alumno y ranking con persistencia.
- Certificado de médico que desbloquee rol o equipo.

## Créditos

- Sistema médico: [ACE-Anvil](https://github.com/acemod/ACE-Anvil) (ACE Mod Team). Este mod no incluye código de ACE; solo usa su API como dependencia.
- API de Arma Reforger: [Script API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/index.html).
