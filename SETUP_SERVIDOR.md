# Guía de setup para dueños de servidor

Esta guía es para quien tiene un servidor de Arma Reforger y quiere añadirle las **prácticas de medicina**. Para modificar el código del mod, mirad el [README](README.md).

## Antes de empezar

El mod **no funciona solo con añadirlo a la lista de mods del servidor**. Ya trae el **herido** y el **instructor** como prefabs, pero el instructor, el gestor y el punto de aparición tienen que estar colocados en vuestro mapa. Eso se hace una vez en Workbench, dentro de **vuestro propio mod de escenario**, que es el que cargará el servidor.

```
Vuestro mod de escenario  (mapa + instructor + gestor + arsenal)
 └── depende de: Medical Training
      └── depende de: ACE Core, ACE Medical Core, Hitzones, Breathing, Circulation
```

Si ya tenéis un mod propio para el servidor, con vuestro mapa o escenario, usad ese y saltad al paso 3.

**Necesitáis:**

- Arma Reforger Tools (gratis en Steam, en *Biblioteca → Herramientas*)
- Cuenta de Bohemia, para publicar en el Workshop
- Acceso al `config.json` del servidor
- 30–60 minutos la primera vez

## 1. Crear vuestro proyecto

1. Abrid **Arma Reforger Tools**.
2. En el launcher: **Add Project → Create New Project**.
3. Ponedle nombre (por ejemplo `MiServidor_Escenario`) y cread el proyecto.

## 2. Añadir las dependencias

1. En el launcher, seleccionad el proyecto y abrid sus ajustes (o, ya dentro, *Workbench → Options → Game Project → Dependencies*).
2. Añadid **Medical Training** desde el Workshop. Las dependencias de ACE se añaden con él; si no, añadid a mano ACE Core, ACE Medical Core, ACE Medical Hitzones, ACE Medical Breathing y ACE Medical Circulation.
3. Abrid el proyecto. Si al cargar sale algún error en rojo con `ACE_Medical_*` en la consola, las versiones de ACE no coinciden: avisad abriendo un issue en este repo.

## 3. Preparar el mapa

### 3.1 Abrir o crear el mundo

- **Si ya tenéis un mundo propio:** abridlo en el **World Editor**.
- **Si usáis un mapa oficial (Everon, Arland…):** no lo modifiquéis, cread una **sub-escena** encima:
  1. Abrid el **World Editor** y cargad el mapa (Everon es `Eden`).
  2. **New → Sub-scene (of current world)**.
  3. Guardadla en vuestro proyecto, por ejemplo en `Worlds/MiServidor.ent`.

### 3.2 Punto de aparición del herido

1. Elegid la zona de prácticas (tienda médica, camilla…).
2. Colocad una entidad vacía (`GenericEntity`) donde queráis que aparezca el herido, orientada como queráis que quede tumbado.
3. En sus propiedades, poned el **Name** exactamente: `MedTraining_Spawn`

### 3.3 Herido

No hay que colocarlo ni crear nada: el mod trae el prefab `Prefabs/Training/MedTraining_Patient.et` y el gestor lo hace aparecer en `MedTraining_Spawn` cuando empieza una práctica. Solo tenéis que seleccionarlo en el gestor (paso 3.4).

### 3.4 Gestor de la práctica

1. En la lista de entidades del World Editor, buscad `MedicalTraining` (todo junto). Está en **TAG → Training**.
2. Arrastrad `TAG_MedicalTrainingManager` al mapa. Es invisible y la posición da igual.
3. **Add Component → `RplComponent`**. Es obligatorio en servidor dedicado.
4. En *Object Properties* rellenad al menos:

| Campo | Valor |
| --- | --- |
| Patient Prefab | `Prefabs/Training/MedTraining_Patient.et` (incluido en el mod) |
| Spawn Point Name | `MedTraining_Spawn` |
| Task Prefab | Un `SCR_Task` básico de `Prefabs/Tasks/` (clic derecho → *Copy Resource Name*) |

El resto ya trae valores por defecto: 7 minutos de límite, un escenario por herido y todos los escenarios activados. La tabla completa de opciones está en el [README](README.md#7-colocar-el-gestor).

**Solo un gestor por mapa.**

### 3.5 Instructor

1. En el **Resource Browser**, buscad `Prefabs/Training/MedTraining_Instructor.et`.
2. Arrastradlo al mapa junto a la zona de prácticas. Ya trae las acciones **Iniciar práctica de medicina** y **Cancelar práctica de medicina**; no hay que configurar nada.

### 3.6 Material médico

Poned al lado un arsenal con material de ACE: vendas, torniquetes, morfina, epinefrina, salino, chest seals, kits NCD y cánulas.

Guardad el mundo con **Ctrl+S**.

## 4. Mission header

El servidor carga el escenario a través de un archivo `.conf`.

- **Si ya tenéis escenario:** no hace falta nada, el mundo es el mismo.
- **Si es nuevo:** en el World Editor, **Plugins → Game Mode Setup**, seguid el asistente y pulsad **Create Header**. Rellenad nombre, descripción y jugadores máximos.

Apuntad la ruta del `.conf` resultante, por ejemplo `Missions/MiServidor.conf`. Para ver su GUID, haced clic derecho sobre él en el Resource Browser → **Copy Resource Name**. Os dará algo como `{A1B2C3D4E5F60718}Missions/MiServidor.conf`.

## 5. Probar en local

1. Con el mundo abierto, pulsad **Play**.
2. Hablad con el instructor → **Iniciar práctica de medicina**.
3. Tratad al herido y comprobad que la tarea se completa. En la consola debe salir una línea `[MedTraining] Jugador ... | aprobado: 1 | ...`.

Para probar varios escenarios, poned *Max Scenarios* a 1 y activad solo uno cada vez.

## 6. Publicar vuestro mod

1. En Workbench: **Publish Project**.
2. Iniciad sesión con la cuenta de Bohemia y rellenad nombre, descripción y miniatura.
3. La visibilidad puede ser **privada o no listada** si solo es para vuestro servidor.
4. Copiad el **ID del mod** desde su página del Workshop.

## 7. Configurar el servidor

En el `config.json` del servidor:

```json
{
  "game": {
    "scenarioId": "{A1B2C3D4E5F60718}Missions/MiServidor.conf",
    "mods": [
      {
        "modId": "ID_DE_VUESTRO_MOD",
        "name": "MiServidor_Escenario"
      }
    ]
  }
}
```

- **`scenarioId`:** el Resource Name del paso 4.
- **`mods`:** basta con vuestro mod. El servidor descarga solo las dependencias (Medical Training y ACE), y a los jugadores se les bajan al entrar.
- Si ya teníais más mods en la lista, dejadlos y añadid el vuestro.

Reiniciad el servidor.

## 8. Comprobar que funciona

Entrad al servidor y probad una práctica. Si algo falla, mirad el log del servidor (`console.log`) y buscad `[MedTraining]`:

| Mensaje | Qué hacer |
| --- | --- |
| `No existe el punto MedTraining_Spawn` | El *Name* de la entidad del paso 3.2 está mal escrito |
| `No se pudo crear el herido` | *Patient Prefab* vacío en el gestor: seleccionad `MedTraining_Patient.et` |
| `El herido no tiene ACE_Medical_VitalsComponent` | Falta ACE Breathing/Circulation en las dependencias |
| `No existe la zona ACE_Medical_LFemoralArtery` | Falta ACE Medical Hitzones |
| No aparece la acción en el instructor | Usad el prefab `MedTraining_Instructor.et` del mod tal cual; si lo habéis duplicado o modificado, revisad su `ActionsManagerComponent` |
| La acción no se bloquea para otros jugadores | Falta el `RplComponent` en el gestor |

## Actualizaciones

Cuando Medical Training se actualice en el Workshop, el servidor descarga la nueva versión al reiniciar. Solo tendréis que volver a publicar vuestro mod si cambiáis algo del mapa.

## Preguntas o errores

Abrid un [issue](../../issues) con el mensaje de la consola y la versión de ACE que usáis.
