# Аудио

## Основные понятия

### Бекенд (Backend)

Вариант внутренней реализации звуковой подсистемы, управляющий выводом звука.
На данный момент в движке существует два:

- `NoAudio` - заглушка. Используется при неработающей аудиосистеме.
- `ALAudio` - основной вариант. Используется для вывода звука через OpenAL.

> [!NOTE] Дополнение к NoAudio
>
> Аудиосистема может не работать по 2 причинам:
>
> - Невозможность инициализации OpenAL.
> - Аудиосистема отключенной через файл настроек: `[audio] enabled=false`.
>
> ---
>
> Данный бекенд загружает PCM данные только по требованию,
> не создает спикеров при попытке воспроизведения аудио.

### Канал (Channel)

Определяет категорию источников аудио для регулирования громкости, наложения эффектов, паузы.
Управление каналам происходит автоматически самим движком.
На данный момент существует следующий набор каналов:

- `master` - управляет громкостью остальных каналов.
- `ui` - звуки интерфейса
- `regular` - звуки игрового мира, ставятся на паузу вместе с игрой.
- `ambient` - то же, что и regular, но предназначается для фоновых звуков: погода и иной эмбиент.
- `music` - канал для воспроизведения музыки. Как правило, потокового аудио.

> [!TIP] Дополнение к master
> Не следует указывать как целевой канал при воспроизведении аудио.

### Звук (Sound)

Звуковые данные загруженные в память для возможности
одновременного воспроизведения из нескольких источников.
Может предоставлять доступ к PCM данным.

### Поток (Stream)

Потоковое аудио. Не загружается полностью в память,
поэтому не требует предзагрузки через `preload.json`.
Не может воспроизводиться через несколько спикеров одновременно.

### Источник PCM (PCMStream)

Поток, используемый потоком как источник PCM-данных.
Реализация зависит не от бекенда звуковой системы, а от формата файла.
Реализация потокового аудио из сетевого соединения делается через реализацию данного интерфейса.

### Спикер (Speaker)

Одноразовый контроллер проигрываемого аудио: звука или потока. Спикер уничтожается после остановки через вызов метода **stop** или при окончании аудио (поток также не удерживает спикер от уничтожения).
Контроллер продолжает жить при паузе.

> [!NOTE] Дополнение
Доступ к спикерам производится по целочисленным id, которые не повторяются за время работы движка, следует избегать хранения прямых указателей на объекты класса.

Нумерация ID спикеров начинается с 1. ID 0 означает невозможность воспроизведения, по какой-либо причине.

## Поддержка форматов

На данный момент реализована поддержка двух форматов:

- WAV - поддерживаются 8 и 16 bit (24 bit не поддерживается OpenAL)
- OGG - реализовано через библиотеку libvorbis

## Дополнительно

> [!WARNING] Внимание
> При воспроизведении через OpenAL стерео звуки не будут учитывать расположение источников относительно игрока. Звуки, которые должны учитывать расположение, должны быть в моно.

## API аудио в скриптинге

### Воспроизведение аудио

Работа с аудио производится при помощи библиотеки `audio`.

---

`play_stream` воспроизводит потоковое аудио из указанного файла, на указанной позиции в мире. Возвращает id спикера.

```lua
audio.play_stream(
    -- путь к аудио-файлу
    -- string
    name,

    -- позиция источника аудио в мире
    -- number, number, number
    x, y, z,

    -- громкость аудио (от 0.0 до 1.0)
    -- number
    volume,

    -- скорость воспроизведения (положительное число)
    -- number
    pitch,

    -- [опционально] имя канала: regular/ambient/music/ui (по-умолчанию - regular)
    -- string
    channel,

    -- [опционально] зацикливание потока (по-умолчанию - false)
    -- boolean
    loop
) --> return number(integer)
```

---

`play_stream_2d` воспроизводит потоковое аудио из указанного файла. Возвращает id спикера.

```lua
audio.play_stream_2d(
    -- путь к аудио-файлу
    -- string
    name,

    -- громкость аудио (от 0.0 до 1.0)
    -- number
    volume,
    
    -- скорость воспроизведения (положительное число)
    -- number
    pitch,

    -- [опционально] имя канала: regular/ambient/music/ui (по-умолчанию - regular)
    -- string
    channel,
    
    -- [опционально] зацикливание потока (по-умолчанию - false)
    -- boolean
    loop
) --> return number(integer)
```

---

`play_sound` воспроизводит звук на указанной позиции в мире. Возвращает id спикера.

```lua
audio.play_sound(
    -- имя звука
    -- string
    name: string,

    -- позиция источника аудио в мире
    -- number, number, number
    x, y, z,

    -- громкость аудио (от 0.0 до 1.0)
    -- number
    volume,
    
    -- скорость воспроизведения (положительное число)
    -- number
    pitch,

    -- [опционально]
    -- имя канала: regular/ambient/music/ui (по-умолчанию - regular)
    -- string
    channel,

    -- [опционально]
    -- зацикливание потока (по-умолчанию - false)
    -- boolean
    loop
) --> return number(integer)
```

>[!TIP] Дополнение к name
> название загруженного звука указывается без префикса паки (без `sounds/`), номера варианта (`_1`, `_2`, ...), расширения (`.wav`, `.ogg`). 
> Пример: у нас есть `sounds/steps/stone.ogg` передаём `steps/stone` для проигрывания звука или любого из его вариантов.
> Вариант звука выбирается случайно.

---

`play_sound_2d` воспроизводит звук. Возвращает id спикера.

```lua
audio.play_sound_2d(
    -- имя звука
    -- string
    name,

    -- громкость аудио (от 0.0 до 1.0)
    -- number
    volume,

    -- скорость воспроизведения (положительное число)
    -- number
    pitch,

    -- [опционально]
    -- имя канала: "regular"/"ambient"/"music"/"ui" (по-умолчанию - "regular")
    -- string
    channel,

    -- [опционально]
    -- зацикливание потока (по-умолчанию - false)
    -- boolean
    loop
) --> return number(integer)
```

>[!TIP] Дополнение к name
> название загруженного звука указывается без префикса паки (без `sounds/`), номера варианта (`_1`, `_2`, ...), расширения (`.wav`, `.ogg`). 
> Пример: у нас есть `sounds/steps/stone.ogg` передаём `steps/stone` для проигрывания звука или любого из его вариантов.
> Вариант звука выбирается случайно.

---

### Взаимодействие со спикером

При обращении к несуществующим спикером ничего происходить не будет.

```lua
-- остановить воспроизведение спикера
audio.stop(
    -- number(integer)
    speakerid
)


-- поставить спикер на паузу
audio.pause(
    -- number(integer)
    speakerid
)


-- снять спикер с паузы
audio.resume(
    -- number(integer)
    speakerid
)


-- установить зацикливание аудио
audio.set_loop(
    -- number(integer)
    speakerid,

    -- boolean
    state
)


-- проверить, зациклено ли аудио (false если не существует)
audio.is_loop(
    -- number(integer)
    speakerid
)
-- return booleab


-- получить громкость спикера (0.0 если не существует)
audio.get_volume(
    -- number(integer)
    speakerid
) --> return number


-- установить громкость спикера
audio.set_volume(
    -- number(integer)
    speakerid,
    
    -- number
    volume
)


-- получить скорость воспроизведения (1.0 если не существует)
audio.get_pitch(
    -- number(integer)
    speakerid
) --> return number


-- установить скорость воспроизведения
audio.set_pitch(
    -- number(integer)
    speakerid,
    
    -- number
    pitch
)


-- получить временную позицию аудио в секундах (0.0 если не существует)
audio.get_time(
    -- number(integer)
    speakerid
) --> return number


-- установить временную позицию аудио в секундах
audio.set_time(
    -- number(integer)
    speakerid,
    
    -- number
    time
)


-- получить позицию источника звука в мире (nil если не существует)
audio.get_position(
    -- number(integer)
    speakerid
) --> return number, number, number


-- установить позицию источника звука в мире
audio.set_position(
    -- number(integer)
    speakerid,
    
    -- number, number, number
    x, y, z
)


-- получить скорость движения источника звука в мире (nil если не существует)
-- (используется OpenAL для имитации эффекта Доплера)
audio.get_velocity(
    -- number(integer)
    speakerid
) --> return number, number, number


-- установить скорость движения источника звука в мире
-- (используется OpenAL для имитации эффекта Доплера)
audio.set_velocity(
    -- number(integer)
    speakerid,
    
    -- number, number, number
    x, y, z
)


-- получить длительность аудио в секуднах, проигрываемого источником
-- возвращает 0, если не спикер не существует
-- так же возвращает 0, если длительность неизвестна (пример: радио)
audio.get_duration(
    -- number(integer)
    speakerid
) --> return number
```


### Другие функции

```lua
-- получить текущее число живых спикеров
audio.count_speakers() --> return integer

-- получить текущее число проигрываемых аудио-потоков
audio.count_streams() -> integer
```