# Tekken Battle DSL Compiler in C++

## How to Compile and Run

### Compile

```bash
g++ Tekken.cpp -o tekken
```

or (recommended for C++11 support):

```bash
g++ -std=c++11 Tekken.cpp -o tekken
```

### Run

```bash
./tekken
```

---

## Project Overview

This project implements a domain-specific language (DSL) for simulating battles inspired by the Tekken video game. The language is embedded in C++ and allows users to define fighters, abilities, and duels using a custom syntax.

The DSL is implemented using macros, operator overloading, and helper classes.

---

## Features

### Fighter Definition

Fighters are defined with:
- Name
- Type
- Health Points (HP)

Supported types:
- Rushdown
- Grappler
- Heavy
- Evasive

Example:

```cpp
CREATE FIGHTER {
    NAME: "King",
    TYPE: "Grappler",
    HP: 150
}
```

---

### Ability Definition

Abilities define actions that fighters can perform.

Each ability includes:
- A name
- An ACTION block

Example:

```cpp
CREATE ABILITY {
    NAME: "Head_Smash",
    ACTION: START
        DAMAGE DEFENDER 22
    END
}
```

---

### Supported Actions

Inside abilities:

- DAMAGE DEFENDER/ATTACKER number  
- HEAL DEFENDER/ATTACKER number  
- TAG DEFENDER/ATTACKER value  

Utility functions:
- `GET_HP(...)`
- `GET_TYPE(...)`
- `GET_NAME(...)`
- `IS_OUT_OF_RING(...)`

---

### Control Flow

#### Conditionals

```cpp
IF condition DO
    ...
ELSE_IF condition DO
    ...
ELSE
    ...
END
```

#### Loops

```cpp
FOR 5 ROUNDS DO
    ...
END
```

#### Delayed Actions

```cpp
AFTER 2 ROUNDS DO
    ...
END
```

#### Output

```cpp
SHOW ...
```

---

### Assigning Abilities

Fighters must learn abilities:

```cpp
DEAR "Lee" LEARN [
    ABILITY_NAME(Give_Autographs)
    ABILITY_NAME(Head_Smash)
]
```

---

## Battle Mechanics

Each fighter type has special behavior:

- Heavy: receives reduced damage  
- Rushdown: deals increased damage  
- Evasive: deals slightly more damage and receives less  
- Grappler: gains bonuses depending on round number  

Additional rules:

- Effects can be immediate or delayed  
- Delayed actions (`AFTER N ROUNDS`) execute later  
- `TAG` controls whether a fighter is in or out of the ring  
- Type modifiers are applied automatically during combat  

---

## Duel System

The `DUEL` command starts the fight.

Flow:

1. Fighters are displayed  
2. Players choose fighters  
3. Fighters enter the ring  
4. Each round, players select abilities  
5. Status updates after each action  
6. The fight ends when a fighter reaches 0 HP  

---

## Example Program

```cpp
#include "Tekken.h"

BEGIN_GAME

CREATE ABILITY {
    NAME:"Give_Autographs",
    ACTION: START
        TAG DEFENDER ---
        AFTER 2 ROUNDS DO
            TAG DEFENDER _
        END
    END
}

CREATE ABILITY {
    NAME: "Head_Smash",
    ACTION: START
        DAMAGE DEFENDER 22
    END
}

CREATE FIGHTER {
    NAME: "Lee",
    TYPE: "Rushdown",
    HP: 100
}

CREATE FIGHTER {
    NAME: "Jack-6",
    TYPE: "Heavy",
    HP: 90
}

DEAR "Lee" LEARN [
    ABILITY_NAME(Give_Autographs)
    ABILITY_NAME(Head_Smash)
]

DEAR "Jack-6" LEARN [
    ABILITY_NAME(Head_Smash)
]

DUEL

END_GAME
```

---

## Example Output

```text
Player 1 chooses Lee
Player 2 chooses Jack-6

Round 1:
Lee uses Head_Smash
Jack-6 takes damage

Round 2:
Jack-6 uses Head_Smash
Lee takes damage

Winner: Lee
```

---

## Implementation Details

- Uses C++ macros to simulate DSL syntax  
- Uses operator overloading for expressive constructs  
- Uses helper classes and objects  
- Compatible with C++11  
