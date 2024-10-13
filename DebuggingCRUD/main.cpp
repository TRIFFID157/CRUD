#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Structure to represent a Workout
struct Workout {
    int id;
    string date;
    string type;
    int duration;
    int successfulReps; // Changed from caloriesBurned to successfulReps
    int sets; // New field for number of sets
    int repsPerSet; // New field for number of reps per set
};

// Data structure to hold workout types and home-friendly exercises
map<int, pair<string, vector<string>>> workoutTypes = {
    {1, {"Cardio", {"Jumping Jacks", "High Knees", "Burpees"}}},
    {2, {"Strength Training", {"Push-Ups", "Bodyweight Squats", "Lunges"}}},
    {3, {"Flexibility", {"Forward Bend", "Cat-Cow Stretch", "Seated Hamstring Stretch"}}},
    {4, {"Balance", {"Single-Leg Stand", "Heel-to-Toe Walk", "Tree Pose"}}},
    {5, {"Core", {"Plank", "Russian Twists", "Bicycle Crunches"}}},
    {6, {"Functional Training", {"Kettlebell Swings (with a dumbbell)", "Bodyweight Deadlifts", "Wall Sit"}}},
    {7, {"Sports-Specific Training", {"Agility Ladder Drills (using tape)", "Shadow Boxing", "Dribbling Drills (using a ball)"}}},
    {8, {"Circuit Training", {"Burpees", "Mountain Climbers", "Jump Squats"}}},
    {9, {"Interval Training", {"Sprints in place", "Tabata Squats", "Battle Ropes (using a towel)"}}},
    {10, {"Mind-Body Workouts", {"Yoga Flow", "Meditation", "Tai Chi"}}}
};

// Constants
const string FILENAME = "workouts.txt";

// Function declarations
int get_int(const string& prompt);
string get_string(const string& prompt);
int generate_unique_id(const map<int, Workout>& workoutMap);
void saveAllWorkoutsToFile(const vector<Workout>& workouts);
void loadWorkoutsFromFile(vector<Workout>& workouts, map<int, Workout>& workoutMap);
void updateWorkout(vector<Workout>& workouts, map<int, Workout>& workoutMap);
void deleteWorkout(vector<Workout>& workouts, map<int, Workout>& workoutMap);
void createWorkout(vector<Workout>& workouts, map<int, Workout>& workoutMap);
void displayAllWorkouts(const vector<Workout>& workouts);
void displayWorkoutOptions();
void displayExercises(int workoutType);
void searchWorkoutByID(const map<int, Workout>& workoutMap, int id);

// History log
vector<string> historyLog; // Store history logs

// Function to display a styled menu
void displayMenu() {
    cout << "+---------------------------------------------+\n";
    cout << "|              Home Workout Tracker           |\n";
    cout << "+---------------------------------------------+\n";
    cout << "| 1. Add Workout                              |\n";
    cout << "| 2. Display All Workouts                     |\n";
    cout << "| 3. Search Workout by ID                     |\n"; // Added search option
    cout << "| 4. Update a Workout                         |\n";
    cout << "| 5. Delete a Workout                         |\n";
    cout << "| 6. Exit                                     |\n";
    cout << "+---------------------------------------------+\n";
}

int main() {
    vector<Workout> workoutsList;          // List of workouts
    map<int, Workout> workoutMap;          // Map for quick lookup by ID

    // Load existing workouts from file into memory
    loadWorkoutsFromFile(workoutsList, workoutMap);

    int choice;

    // Main menu loop
    do {
        // Display the menu with design
        displayMenu();

        // Get the user's choice
        choice = get_int("\n>> ");

        // Handle the user's choice
        switch (choice) {
            case 1:
                createWorkout(workoutsList, workoutMap);
                break;
            case 2:
                displayAllWorkouts(workoutsList);
                break;
            case 3: {
                int searchId = get_int("Enter workout ID to search: ");
                searchWorkoutByID(workoutMap, searchId); // Search by ID
                break;
            }
            case 4:
                updateWorkout(workoutsList, workoutMap);
                break;
            case 5:
                deleteWorkout(workoutsList, workoutMap);
                break;
            case 6:
                cout << "Exiting the program. Have a great day!\n";
                return 0;
            default:
                cout << "Invalid input. Please try again.\n";
        }

        // Prompt user to press any key to continue and clear the screen
        cout << "\nEnter any key to continue...";
        cin.get(); // Wait for key press
        system("CLS"); // Clear the screen (Windows-specific)
    } while (choice != 6);

    return 0;
}

// Get valid integer input from the user
int get_int(const string& prompt) {
    regex integer_regex("^-?[0-9]+$");
    string input;

    while (true) {
        cout << prompt;
        getline(cin, input);

        if (regex_match(input, integer_regex)) {
            try {
                return stoi(input); // Convert string to integer
            } catch (out_of_range&) {
                cout << "Error: Number out of range. Please try again.\n";
            }
        } else {
            cout << "Invalid input. Please enter a valid integer.\n";
        }
    }
}

// Get valid string input from the user
string get_string(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}

// Generate a unique workout ID
int generate_unique_id(const map<int, Workout>& workoutMap) {
    srand(time(0)); // Seed the random number generator
    int id;

    do {
        id = rand() % 9000 + 1000; // Generate a random ID between 1000 and 9999
    } while (workoutMap.find(id) != workoutMap.end()); // Ensure the ID is unique

    return id;
}

// Save all workouts to a file
void saveAllWorkoutsToFile(const vector<Workout>& workouts) {
    ofstream outFile(FILENAME, ios::trunc); // Open the file in truncation mode
    if (outFile) {
        for (const auto& workout : workouts) {
            outFile << "id: " << workout.id << '\n'
                    << "date: " << workout.date << '\n'
                    << "type: " << workout.type << '\n'
                    << "duration: " << workout.duration << '\n'
                    << "successful_reps: " << workout.successfulReps << '\n' // Changed from caloriesBurned to successfulReps
                    << "sets: " << workout.sets << '\n' // New field for sets
                    << "reps_per_set: " << workout.repsPerSet << "\n\n"; // New field for reps per set
        }
        outFile.close();
    } else {
        cout << "Error: Could not save workouts to file.\n";
    }
}

// Load workouts from a file into memory
void loadWorkoutsFromFile(vector<Workout>& workouts, map<int, Workout>& workoutMap) {
    ifstream inFile(FILENAME);
    if (inFile) {
        Workout workout;
        string line;
        while (getline(inFile, line)) {
            if (line.find("id: ") != string::npos) {
                workout.id = stoi(line.substr(4));
            } else if (line.find("date: ") != string::npos) {
                workout.date = line.substr(6);
            } else if (line.find("type: ") != string::npos) {
                workout.type = line.substr(6);
            } else if (line.find("duration: ") != string::npos) {
                workout.duration = stoi(line.substr(10));
            } else if (line.find("successful_reps: ") != string::npos) { // Changed from calories: to successful_reps:
                workout.successfulReps = stoi(line.substr(18)); // Updated index for successfulReps
            } else if (line.find("sets: ") != string::npos) { // New field for sets
                workout.sets = stoi(line.substr(6));
            } else if (line.find("reps_per_set: ") != string::npos) { // New field for reps per set
                workout.repsPerSet = stoi(line.substr(15));
                workouts.push_back(workout); // Add the workout to the list
                workoutMap[workout.id] = workout; // Add to the map
            }
        }
        inFile.close();
    }
}

// Create a new workout
void createWorkout(vector<Workout>& workouts, map<int, Workout>& workoutMap) {
    Workout newWorkout;
    newWorkout.id = generate_unique_id(workoutMap);
    newWorkout.date = get_string("Enter workout date (YYYY-MM-DD): ");

    displayWorkoutOptions();
    int typeChoice = get_int("Choose workout type (1-10): ");

    if (workoutTypes.find(typeChoice) != workoutTypes.end()) {
        newWorkout.type = workoutTypes[typeChoice].first;
        displayExercises(typeChoice);

        int exerciseChoice = get_int("Choose an exercise (1-" + to_string(workoutTypes[typeChoice].second.size()) + "): ");

        if (exerciseChoice > 0 && exerciseChoice <= workoutTypes[typeChoice].second.size()) {
            cout << "You chose: " << workoutTypes[typeChoice].second[exerciseChoice - 1] << "\n";
        }

        newWorkout.sets = get_int("Enter number of sets: "); // New input for sets
        newWorkout.repsPerSet = get_int("Enter number of reps per set: "); // New input for reps per set

        newWorkout.successfulReps = newWorkout.sets * newWorkout.repsPerSet; // Calculate successful reps as total reps
        newWorkout.duration = get_int("Enter workout duration (in minutes): ");

        workouts.push_back(newWorkout);
        workoutMap[newWorkout.id] = newWorkout; // Add to the map
        saveAllWorkoutsToFile(workouts); // Save updated workouts to file
    } else {
        cout << "Invalid workout type chosen. Please try again.\n";
    }
}

// Display all workouts
void displayAllWorkouts(const vector<Workout>& workouts) {
    cout << "+------------------------------+\n";
    cout << "|          All Workouts        |\n";
    cout << "+------------------------------+\n";

    for (const auto& workout : workouts) {
        cout << "ID: " << workout.id << ", Date: " << workout.date
             << ", Type: " << workout.type << ", Duration: "
             << workout.duration << " mins, Successful Reps: "
             << workout.successfulReps << ", Sets: " << workout.sets
             << ", Reps per Set: " << workout.repsPerSet << "\n"; // Display sets and reps
    }
    cout << "+------------------------------+\n";
}

// Display workout options
void displayWorkoutOptions() {
    cout << "+---------------------------+\n";
    cout << "|      Workout Options      |\n";
    cout << "+---------------------------+\n";
    for (const auto& pair : workoutTypes) {
        cout << pair.first << ". " << pair.second.first << "\n"; // Display workout type
    }
    cout << "+---------------------------+\n";
}

// Display exercises based on workout type
void displayExercises(int workoutType) {
    cout << "+---------------------------+\n";
    cout << "|      Exercises for " << workoutTypes[workoutType].first << "      |\n";
    cout << "+---------------------------+\n";
    const auto& exercises = workoutTypes[workoutType].second;
    for (size_t i = 0; i < exercises.size(); ++i) {
        cout << (i + 1) << ". " << exercises[i] << "\n"; // Display exercises
    }
    cout << "+---------------------------+\n";
}

// Search for a workout by ID
void searchWorkoutByID(const map<int, Workout>& workoutMap, int id) {
    auto it = workoutMap.find(id);
    if (it != workoutMap.end()) {
        const Workout& workout = it->second;
        cout << "+---------------------------+\n";
        cout << "|      Workout Found        |\n";
        cout << "+---------------------------+\n";
        cout << "ID: " << workout.id << ", Date: " << workout.date
             << ", Type: " << workout.type << ", Duration: "
             << workout.duration << " mins, Successful Reps: "
             << workout.successfulReps << ", Sets: " << workout.sets
             << ", Reps per Set: " << workout.repsPerSet << "\n"; // Display sets and reps
        cout << "+---------------------------+\n";
    } else {
        cout << "No workout found with ID: " << id << ".\n";
    }
}

// Update a workout
void updateWorkout(vector<Workout>& workouts, map<int, Workout>& workoutMap) {
    int idToUpdate = get_int("Enter the ID of the workout to update: ");
    auto it = workoutMap.find(idToUpdate);
    if (it != workoutMap.end()) {
        Workout& workoutToUpdate = it->second;
        cout << "Updating workout: ID: " << workoutToUpdate.id << ", Date: "
             << workoutToUpdate.date << ", Type: " << workoutToUpdate.type
             << ", Duration: " << workoutToUpdate.duration
             << " mins, Successful Reps: " << workoutToUpdate.successfulReps
             << ", Sets: " << workoutToUpdate.sets
             << ", Reps per Set: " << workoutToUpdate.repsPerSet << "\n"; // Display all fields

        workoutToUpdate.date = get_string("Enter new date (YYYY-MM-DD) or press enter to keep the same: ");
        if (workoutToUpdate.date.empty()) {
            workoutToUpdate.date = it->second.date; // Keep the original date if no new date is provided
        }
        workoutToUpdate.duration = get_int("Enter new duration (in minutes) or press -1 to keep the same: ");
        if (workoutToUpdate.duration == -1) {
            workoutToUpdate.duration = it->second.duration; // Keep the original duration
        }
        workoutToUpdate.successfulReps = get_int("Enter new successful reps or press -1 to keep the same: "); // Changed to successful reps
        if (workoutToUpdate.successfulReps == -1) {
            workoutToUpdate.successfulReps = it->second.successfulReps; // Keep the original successful reps
        }
        workoutToUpdate.sets = get_int("Enter new number of sets or press -1 to keep the same: "); // Updated for sets
        if (workoutToUpdate.sets == -1) {
            workoutToUpdate.sets = it->second.sets; // Keep the original sets
        }
        workoutToUpdate.repsPerSet = get_int("Enter new number of reps per set or press -1 to keep the same: "); // Updated for reps per set
        if (workoutToUpdate.repsPerSet == -1) {
            workoutToUpdate.repsPerSet = it->second.repsPerSet; // Keep the original reps per set
        }

        saveAllWorkoutsToFile(workouts); // Save updated workouts to file
    } else {
        cout << "No workout found with ID: " << idToUpdate << ".\n";
    }
}

// Delete a workout
void deleteWorkout(vector<Workout>& workouts, map<int, Workout>& workoutMap) {
    int idToDelete = get_int("Enter the ID of the workout to delete: ");
    auto it = workoutMap.find(idToDelete);
    if (it != workoutMap.end()) {
        workouts.erase(remove_if(workouts.begin(), workouts.end(),
                   [idToDelete](const Workout& workout) { return workout.id == idToDelete; }), workouts.end());
        workoutMap.erase(it); // Remove from map
        saveAllWorkoutsToFile(workouts); // Save updated workouts to file
        cout << "Workout with ID " << idToDelete << " has been deleted.\n";
    } else {
        cout << "No workout found with ID: " << idToDelete << ".\n";
    }
}
