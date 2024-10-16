#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// There will be m players in an array
typedef struct Player {
    int id;
    int loc;
    long balance;
} TPlayer;

// There will be n properties in an array
typedef struct Property {
    int id;
    int owner_id;
    int rent;
} TProperty;

// TODO
// Implement the following function
// The player p1 needs to pay the player p2 'amount' of dollars
// If p1 has enough balance, the transaction will be successful
// Otherwise, p1 will pay all of their balance to p2, and the transaction
// is not successful
// return 1 if the transaction is successful
// Otherwise, return 0
int transaction(TPlayer *p1, TPlayer *p2, int amount) {
    // Check if p1 can afford the amount
    if (p1->balance >= amount) {
        // Successful transaction, p1 pays p2
        p1->balance -= amount;
        p2->balance += amount;
        return 1;  // Return 1 for a successful transaction
    } else {
        // Unsuccessful transaction, p1 gives all they have
        p2->balance += p1->balance;
        p1->balance = 0;  // p1 is now bankrupt
        return 0;  // Return 0 because the transaction failed
    }
}

// TODO
// Finish the following function
// If one player cannot pay rent to another player, this function should return 0. 
// The rest of the players will not get chances to play when this happens 
int one_round(int m, int n, TPlayer p[], TProperty prop[]) {
    for (int i = 0; i < m; i++) {
        // Roll two dice to determine how far the player moves
        int steps = rand() % 6 + 1 + rand() % 6 + 1;
        
        // Update player's location on the property map (circular movement)
        p[i].loc = (p[i].loc + steps) % n;

        // Get the property the player landed on
        TProperty *current_property = &prop[p[i].loc];
        
        if (current_property->owner_id == -1) {
            // The property is unowned, so the player buys it
            current_property->owner_id = p[i].id;
        } else if (current_property->owner_id != p[i].id) {
            // The property is owned by another player, rent must be paid
            int owner_id = current_property->owner_id;
            int rent = current_property->rent;
            
            // Try to make the transaction
            if (!transaction(&p[i], &p[owner_id], rent)) {
                // If the transaction failed, the player is bankrupt and the round ends
                return 0;
            }
        }

        // Award money if the player passed the start (property 0)
        if (p[i].loc == 0) {
            p[i].balance += n;  // Award n dollars
        }
    }
    return 1;  // Return 1 to indicate the round was successful
}

// used for printing out results
void print_result(int m, TPlayer p[]) {
    printf("      id    balance\n");
    for (int i = 0; i < m; i++) {
        printf("%8d %8ld\n", p[i].id, p[i].balance);
    }

    long sum = 0;
    long max = 0;
    for (int i = 0; i < m; i++) {
        sum += p[i].balance;
        if (p[i].balance > max) max = p[i].balance;
    }
    printf("average: %f max: %ld, max/average = %lf\n", (double)sum/m, max, (double)max*m/sum); 
}

// max_rounds is needed because the game may never finish
void monopoly(int m, int n, TPlayer p[], TProperty prop[], int max_rounds) {
    srand(12345);  // Seed for consistent random behavior
    int rounds = 1;

    // Loop through the rounds, up to the maximum number of rounds
    while (one_round(m, n, p, prop) && rounds < max_rounds) {
        rounds++;
    }

    // Print the results after the game ends
    print_result(m, p);
    printf("after %d rounds\n", rounds);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s m n rounds\n", argv[0]);
        return -1;
    }

    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    int rounds = atoi(argv[3]);

    // Ensure valid input
    assert(n >= 13);
    assert(m >= 1);
    assert(rounds >= 1);

    // Create player and property arrays
    TPlayer p[m];
    TProperty prop[n];

    // Initialize properties
    for (int i = 0; i < n; i++) {
        prop[i].id = i;
        prop[i].owner_id = -1;  // No owner at the start
        prop[i].rent = i + 1;   // Rent increases as property ID increases
    }

    // Initialize players
    for (int j = 0; j < m; j++) {
        p[j].id = j;
        p[j].loc = 0;  // All players start at property 0
        p[j].balance = n;  // All players start with n dollars
    }

    // Start the game
    monopoly(m, n, p, prop, rounds);
    return 0;
}
