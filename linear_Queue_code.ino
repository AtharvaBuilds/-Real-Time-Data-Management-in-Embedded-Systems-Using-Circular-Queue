#include <DHT.h>

#define DHTPIN 2          // Pin where the DHT sensor is connected
#define DHTTYPE DHT11     
#define MAX_QUEUE_SIZE 5  // Define maximum queue size

DHT dht(DHTPIN, DHTTYPE);  // Initialize DHT sensor

class LinearQueue 
{
private:
    struct node 
    {
        float data;
        node* next;
    };

    node* head;  // Pointer to the front of the queue
    node* tail;  // Pointer to the end of the queue
    int queueSize;  // Track the current size of the queue

public:
    LinearQueue() : 
    head(NULL), tail(NULL), queueSize(0) {}  // Constructor

    // Enqueue function with timing
    void enqueue(float value) 
    {
        unsigned long startTime = micros();  // Start time

        node* temp = new node();
        temp->data = value;
        temp->next = NULL;

        if (queueSize < MAX_QUEUE_SIZE)
        {
            // If queue is not full, add new data normally
            if (head == NULL) 
            {  // Empty queue
                head = temp;
                tail = temp;
            } 
            else 
            {
                tail->next = temp;
                tail = temp;
            }
            queueSize++;  // Increment queue size
        } 
        else 
        {
            // If queue is full, remove the oldest data and enqueue new data
            Serial.print("\n\tQueue Full: Removing oldest: ");
            Serial.print(head->data);
            Serial.println(" °C");

            node* tempHead = head;
            head = head->next;
            delete tempHead;  // Free memory of the removed node

            tail->next = temp;
            tail = temp;

            Serial.print("\n\tOldest reading has been removed. Enqueued: ");
            Serial.print(value);
            Serial.println(" °C");
        }

        unsigned long endTime = micros();  // End time
        unsigned long enqueueTime = endTime - startTime;  // Time taken

        Serial.print("\n\tEnqueued Temperature: ");
        Serial.print(value);
        Serial.println(" °C");
        Serial.print("\tEnqueue Time: ");
        Serial.print(enqueueTime);  // Time in microseconds
        Serial.println(" μs");
    }

    // Function to display the queue contents with timing
    void displayQueue() 
    {
        unsigned long startTime = micros();  // Start time

        if (head == NULL) 
        {
            Serial.println("\n\tQueue is empty.");
            return;
        }

        node* current = head;
        Serial.print("\n\tQueue contents: ");
        while (current != NULL) 
        {
            Serial.print(current->data);
            Serial.print(" °C ");
            current = current->next;
        }
        Serial.println();

        unsigned long endTime = micros();  // End time
        unsigned long displayTime = endTime - startTime;  // Time taken

        Serial.print("\tDisplay Time: ");
        Serial.print(displayTime);  // Time in microseconds
        Serial.println(" μs");
    }

    // Destructor to free memory
    ~LinearQueue() 
    {
        while (head != NULL) 
        {
            node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// Global LinearQueue object
LinearQueue temperatureQueue;

bool exitProgram = false; // Flag to control exit

void setup() 
{
    Serial.begin(9600);
    dht.begin();  // Initialize the DHT sensor
}

void loop() 
{
    // Check if the program should exit
    if (exitProgram) 
    {
        Serial.println("\tProgram exited.");
        while (true); // Infinite loop to stop the program
    }

    // Display menu options
    Serial.println("\n\t\tMemory Efficient Linear Queue for Temperature Data");
    Serial.println("\n\tMenu:");
    Serial.println("\n\t1. Read Temperature");
    Serial.println("\n\t2. Display Queue");
    Serial.println("\n\t3. Exit");
    Serial.print("\n\tChoose an option: ");

    while (Serial.available() == 0) {} // Wait for user input
    int choice = Serial.parseInt(); // Read user choice

    // Clear the Serial buffer
    while (Serial.available() > 0)
    {
        Serial.read();
    }

    Serial.println(choice);

    switch (choice) 
    {
        case 1: 
        {
            // Read temperature as Celsius
            float temperature = dht.readTemperature();

            // Check if the reading is valid
            if (isnan(temperature))
            {
                Serial.println("\n\tFailed to read from DHT sensor!");
            } else 
            {
                temperatureQueue.enqueue(temperature);  // Enqueue the temperature reading
            }
            break;
        }
        case 2:
            temperatureQueue.displayQueue(); // Display the queue
            break;
        case 3:
            Serial.println("\tThank You!!");
            exitProgram = true; // Set exit flag
            delay(1000); // Add delay before exit
            break;
        default:
            Serial.println("\n\tInvalid choice! Please try again.");
            break;
    }

    delay(2000); // Small delay before showing the menu again
}
