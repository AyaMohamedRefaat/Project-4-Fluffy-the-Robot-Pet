#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STAT_MAX      10U
#define LOW_WARNING    3U
#define NAME_LEN      12U

/* mood bits */
#define BIT_ASLEEP     0U
#define BIT_HUNGRY     1U
#define BIT_SAD        2U
#define BIT_SICK       3U

#define SET_BIT(reg, n)    ((reg) |=  (uint8_t)(1U << (n)))
#define CLR_BIT(reg, n)    ((reg) &= (uint8_t)~(1U << (n)))
#define READ_BIT(reg, n)   ((uint8_t)(((reg) >> (n)) & 1U))

typedef struct {
    char     name[NAME_LEN];
    uint8_t  food;      /* 0..10 */
    uint8_t  fun;       /* 0..10 */
    uint8_t  energy;    /* 0..10 */
    uint8_t  mood;      /* the bits above  */
    uint16_t hours;     /* how long Fluffy has been alive */
} Pet_t;

static Pet_t fluffy;

/* Function Prototypes */
static void    hatchPet(void);
static void    drawFace(void);
static void    drawStat(const char *label, uint8_t value);
static void    feed(void);
static void    play(void);
static void    sleepPet(void);
static void    hourPasses(void);
static void    updateMood(void);
static uint8_t isHappy(void);
static void    petReport(void);

int main(void) {
    hatchPet();
    int choice = 0;
    int c = 0;

    do {
        printf("\n====================================\n");
        printf("       FLUFFY THE ROBOT PET         \n");
        printf("====================================\n");
        drawFace();
        printf("1. Feed Fluffy\n");
        printf("2. Play with Fluffy\n");
        printf("3. Toggle Sleep / Wake Up\n");
        printf("4. Pass 1 Hour\n");
        printf("5. Pass 8 Hours (Fast-Forward)\n");
        printf("6. View Care Card Report\n");
        printf("7. Re-hatch Pet\n");
        printf("0. Exit\n");
        printf("------------------------------------\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[!] Invalid input! Please enter a number.\n");
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        switch (choice) {
            case 1: feed(); break;
            case 2: play(); break;
            case 3: sleepPet(); break;
            case 4: hourPasses(); break;
            case 5:
                for (uint8_t i = 0U; i < 8U; ++i) {
                    hourPasses();
                }
                printf("\n[*] 8 hours have passed...\n");
                break;
            case 6: petReport(); break;
            case 7:
                hatchPet();
                printf("\n[*] Fluffy has been re-hatched!\n");
                break;
            case 0: printf("\nGoodbye!\n"); break;
            default: printf("\n[!] Invalid choice.\n"); break;
        }
    } while (choice != 0);

    return 0;
}

static void hatchPet(void) {
    strncpy(fluffy.name, "Fluffy", NAME_LEN - 1U);
    fluffy.name[NAME_LEN - 1U] = '\0';
    fluffy.food = STAT_MAX;
    fluffy.fun = STAT_MAX;
    fluffy.energy = STAT_MAX;
    fluffy.mood = 0U;
    fluffy.hours = 0U;
}

static void drawFace(void) {
    printf("   +---------+\n");
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("   |  (- . -) |\n");
        printf("   |   (zzz)  |\n");
    } else if (READ_BIT(fluffy.mood, BIT_SICK)) {
        printf("   |  (x . x) |\n");
        printf("   |   (~ ~)  |\n");
    } else if (READ_BIT(fluffy.mood, BIT_SAD) || READ_BIT(fluffy.mood, BIT_HUNGRY)) {
        printf("   |  (o . o) |\n");
        printf("   |    (n)   |\n");
    } else {
        printf("   |  (^ . ^) |\n");
        printf("   |    (U)   |\n");
    }
    printf("   +---------+\n");
}

static void drawStat(const char *label, uint8_t value) {
    printf("%-7s [", label);
    for (uint8_t i = 0U; i < STAT_MAX; ++i) {
        if (i < value) {
            putchar('#');
        } else {
            putchar('-');
        }
    }
    printf("] %2u/10\n", value);
}

static void feed(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("\n[!] Fluffy is sleeping! Wake Fluffy up first.\n");
        return;
    }

    if (fluffy.food >= STAT_MAX) {
        SET_BIT(fluffy.mood, BIT_SICK);
        printf("\n[!] Fluffy was already full! Overeating made Fluffy sick.\n");
    } else {
        fluffy.food = (fluffy.food + 3U > STAT_MAX) ? STAT_MAX : (fluffy.food + 3U);
        printf("\n[+] Yum! Fluffy enjoyed the meal.\n");
    }
    updateMood();
}

static void play(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("\n[!] Fluffy is asleep and cannot play right now.\n");
        return;
    }

    fluffy.fun = (fluffy.fun + 3U > STAT_MAX) ? STAT_MAX : (fluffy.fun + 3U);
    
    if (fluffy.energy >= 2U) {
        fluffy.energy -= 2U;
    } else {
        fluffy.energy = 0U;
    }

    printf("\n[+] Played games with Fluffy!\n");
    updateMood();
}

static void sleepPet(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        CLR_BIT(fluffy.mood, BIT_ASLEEP);
        printf("\n[*] Good morning! Fluffy woke up.\n");
    } else {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
        printf("\n[*] Shhh... Fluffy went to sleep.\n");
    }
    updateMood();
}

static void hourPasses(void) {
    fluffy.hours++;

    /* Food decays */
    if (fluffy.food > 0U) {
        fluffy.food--;
    }

    /* Fun decays */
    if (fluffy.fun > 0U) {
        fluffy.fun--;
    }

    /* Energy handling */
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        fluffy.energy = (fluffy.energy + 2U > STAT_MAX) ? STAT_MAX : (fluffy.energy + 2U);
    } else {
        if (fluffy.energy > 0U) {
            fluffy.energy--;
        }
    }

    updateMood();
}

static void updateMood(void) {
    /* Food state */
    if (fluffy.food <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_HUNGRY);
    } else {
        CLR_BIT(fluffy.mood, BIT_HUNGRY);
    }

    /* Fun state */
    if (fluffy.fun <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_SAD);
    } else {
        CLR_BIT(fluffy.mood, BIT_SAD);
    }

    /* Energy exhaustion forces sleep */
    if (fluffy.energy == 0U) {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
    }

    /* Sleep cures sickness */
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        CLR_BIT(fluffy.mood, BIT_SICK);
    }
}

static uint8_t isHappy(void) {
    if (fluffy.food > LOW_WARNING && 
        fluffy.fun > LOW_WARNING && 
        fluffy.energy > LOW_WARNING && 
        fluffy.mood == 0U) {
        return 1U;
    }
    return 0U;
}

static void petReport(void) {
    printf("\n====================================\n");
    printf("         PET CARE CARD              \n");
    printf("====================================\n");
    printf("Name        : %s\n", fluffy.name);
    printf("Hours Alive : %u hrs\n", fluffy.hours);
    printf("Overall     : %s\n", isHappy() ? "HAPPY & HEALTHY" : "NEEDS ATTENTION");
    printf("------------------------------------\n");
    drawStat("Food", fluffy.food);
    drawStat("Fun", fluffy.fun);
    drawStat("Energy", fluffy.energy);
    printf("------------------------------------\n");

    printf("Mood Byte   : 0b");
    for (int8_t i = 7; i >= 0; --i) {
        putchar(READ_BIT(fluffy.mood, (uint8_t)i) ? '1' : '0');
    }
    printf("\n");

    printf("Recommendation: ");
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("Let Fluffy rest and regain energy.\n");
    } else if (READ_BIT(fluffy.mood, BIT_SICK)) {
        printf("Fluffy is sick! Put Fluffy to bed to recover.\n");
    } else if (READ_BIT(fluffy.mood, BIT_HUNGRY)) {
        printf("Fluffy is hungry! Feed Fluffy some food.\n");
    } else if (READ_BIT(fluffy.mood, BIT_SAD)) {
        printf("Fluffy is bored! Play a game with Fluffy.\n");
    } else {
        printf("Fluffy is doing great! Keep up the good work.\n");
    }
    printf("====================================\n");
}