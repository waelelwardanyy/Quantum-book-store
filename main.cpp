#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

using namespace std;

class ShippingService {
public:
    static void ship(const string& address, const string& bookTitle) {
        cout << "[Quantum Book Store] Shipping '" << bookTitle << "' to " << address << endl;
    }
};

class MailService {
public:
    static void sendEmail(const string& email, const string& bookTitle) {
        cout << "[Quantum Book Store] Sending EBook '" << bookTitle << "' to " << email << endl;
    }
};


class Book {
protected:
    string ISBN;
    string title;
    int yearPublished;
    double price;
    string author;

public:
    Book(string ISBN, string title, int year, double price, string author)
        : ISBN(ISBN), title(title), yearPublished(year), price(price), author(author) {}

    virtual ~Book() = default;

    virtual bool isForSale() const { return true; }

    virtual void printInfo() const {
        cout << "[Quantum Book Store] Book: " << title << " by " << author
             << ", Year: " << yearPublished << ", Price: $" << price
             << ", ISBN: " << ISBN << endl;
    }

    string getISBN() const { return ISBN; }
    string getTitle() const { return title; }
    double getPrice() const { return price; }
    int getYear() const { return yearPublished; }

    virtual bool hasStock(int quantity = 1) const { return true; }
    virtual void reduceStock(int quantity) {}
};


class PaperBook : public Book {
private:
    int stock;

public:
    PaperBook(string ISBN, string title, int year, double price, string author, int stock)
        : Book(ISBN, title, year, price, author), stock(stock) {}

    bool hasStock(int quantity) const override {
        return stock >= quantity;
    }

    void reduceStock(int quantity) override {
        stock -= quantity;
    }

    void printInfo() const override {
        Book::printInfo();
        cout << "[Quantum Book Store] Type: PaperBook, Stock: " << stock << endl;
    }
};

class EBook : public Book {
private:
    string fileType;

public:
    EBook(string ISBN, string title, int year, double price, string author, string fileType)
        : Book(ISBN, title, year, price, author), fileType(fileType) {}

    void printInfo() const override {
        Book::printInfo();
        cout << "[Quantum Book Store] Type: EBook, File Type: " << fileType << endl;
    }
};

class ShowcaseBook : public Book {
public:
    ShowcaseBook(string ISBN, string title, int year, double price, string author)
        : Book(ISBN, title, year, price, author) {}

    bool isForSale() const override {
        return false;
    }

    void printInfo() const override {
        Book::printInfo();
        cout << "[Quantum Book Store] Type: Showcase/Demo Book - Not for sale" << endl;
    }
};

class BookStore {
private:
    vector<Book*> inventory;

public:
    ~BookStore() {
        for (auto book : inventory) delete book;
    }

    void addBook(Book* book) {
        inventory.push_back(book);
    }

    void removeOutdatedBooks(int maxAgeInYears) {
        const int CURRENT_YEAR = 2025;
        vector<Book*> updatedInventory;

        for (auto book : inventory) {
            int bookAge = CURRENT_YEAR - book->getYear();
            if (bookAge <= maxAgeInYears) {
                updatedInventory.push_back(book);
            } else {
                cout << "[Quantum Book Store] Removing outdated book: " << book->getTitle() << endl;
                delete book;
            }
        }

        inventory = updatedInventory;
    }

    double buyBook(const string& ISBN, int quantity, const string& email, const string& address) {
        for (auto book : inventory) {
            if (book->getISBN() == ISBN) {
                if (!book->isForSale()) {
                    throw runtime_error("[Quantum Book Store] ERROR: This book is not for sale.");
                }

                if (PaperBook* pb = dynamic_cast<PaperBook*>(book)) {
                    if (!pb->hasStock(quantity)) {
                        throw runtime_error("[Quantum Book Store] ERROR: Not enough stock available.");
                    }
                    pb->reduceStock(quantity);
                    ShippingService::ship(address, book->getTitle());
                } else if (EBook* eb = dynamic_cast<EBook*>(book)) {
                    MailService::sendEmail(email, book->getTitle());
                } else {
                    throw runtime_error("[Quantum Book Store] ERROR: Invalid book type.");
                }

                return book->getPrice() * quantity;
            }
        }

        throw runtime_error("[Quantum Book Store] ERROR: Book with this ISBN not found.");
    }

    void displayInventory() const {
        for (const auto& book : inventory) {
            book->printInfo();
        }
    }
};

int getValidIntInput() {
    int value;
    while (true) {
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[Quantum Book Store] Invalid input. Please enter a number: ";
        } else {
            cin.ignore(1000, '\n'); // Clear extra characters
            return value;
        }
    }
}

class QuantumBookstoreFullTest {
public:
    static void runTest() {
        cout << "[Quantum Book Store] Starting Full System Test...\n";

        BookStore store;

        // 1. Add sample books
        cout << "\n[Quantum Book Store] Adding books to inventory...\n";
        store.addBook(new PaperBook("123", "bule elephant ", 2020, 35.0, "Ahmed Morad", 5));
        store.addBook(new EBook("456", "The Old Man And The Sea", 2021, 25.0, "wael hossam", ".pdf"));
        store.addBook(new ShowcaseBook("789", "Sample Demo Book", 2019, 0.0, "Admin"));

        // 2. Display all books
        displayStepHeader("Current Inventory");
        store.displayInventory();

        // 3. Buy a PaperBook
        testPurchase(store, "123", 2, "customer@mail.com", "123 Main St");

        // 4. Buy an EBook
        testPurchase(store, "456", 1, "customer@mail.com", "");

        // 5. Try to buy a non-existent book
        testPurchase(store, "999", 1, "customer@mail.com", "Nowhere");

        // 6. Show inventory after purchase
        displayStepHeader("Inventory After Purchases");
        store.displayInventory();

        // 7. Remove outdated books (older than 3 years)
        displayStepHeader("Removing Outdated Books (older than 3 years)");
        store.removeOutdatedBooks(3);

        // 8. Final inventory
        displayStepHeader("Final Inventory After Cleanup");
        store.displayInventory();

        cout << "\n[Quantum Book Store] Full test completed successfully!\n";
    }

private:
    // Helper function to simulate purchases
    static void testPurchase(BookStore& store, const string& isbn, int quantity, const string& email, const string& address) {
        cout << "\n[Quantum Book Store] Testing purchase: ISBN=" << isbn << ", Qty=" << quantity << "...\n";
        try {
            double total = store.buyBook(isbn, quantity, email, address);
            cout << "[Quantum Book Store] Purchase successful! Total: $" << total << endl;
        } catch (exception& e) {
            cout << e.what() << endl;
        }
    }

    // Helper function to show section headers
    static void displayStepHeader(const string& title) {
        cout << "\n[Quantum Book Store] === " << title << " ===\n";
    }
};


int main() {
    int choice;
    do {
        cout << "\n--- Quantum Book Store ---\n";
        cout << "Choose Mode:\n";
        cout << "1. Run Full System Test\n";
        cout << "2. Run Interactive Store (Admin/Customer)\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        choice = getValidIntInput();

        switch (choice) {
            case 0:
                cout << "[Quantum Book Store] Thank you for using our system!\n";
                break;

            case 1:
                QuantumBookstoreFullTest::runTest();
                break;

            case 2: {
                BookStore store;

                int mainChoice;
                do {
                    cout << "\n--- Quantum Book Store ---\n";
                    cout << "Are you an Admin or Customer?\n";
                    cout << "1. Admin\n";
                    cout << "2. Customer\n";
                    cout << "0. Exit\n";
                    cout << "Enter your choice: ";
                    mainChoice = getValidIntInput();

                    switch (mainChoice) {
                        case 0:
                            cout << "[Quantum Book Store] Thank you for using our system!\n";
                            break;

                        case 1: {
                            int adminChoice;
                            do {
                                cout << "\n--- Admin Panel ---\n";
                                cout << "1. Add Book\n";
                                cout << "2. Remove Outdated Books\n";
                                cout << "3. View All Books\n";
                                cout << "0. Back to Main Menu\n";
                                cout << "Enter your choice: ";
                                adminChoice = getValidIntInput();

                                switch (adminChoice) {
                                    case 0:
                                        break;

                                    case 1: {
                                        string ISBN, title, author, fileType;
                                        int year, stock;
                                        double price;
                                        int bookType;

                                        cout << "Enter ISBN: ";
                                        cin >> ISBN;
                                        cout << "Enter Title: ";
                                        cin.ignore();
                                        getline(cin, title);
                                        cout << "Enter Author: ";
                                        getline(cin, author);

                                        while (true) {
                                            cout << "Enter Year Published: ";
                                            year = getValidIntInput();
                                            if (year > 2025) {
                                                cout << "[Quantum Book Store] Error: Year cannot be in the future (max allowed: 2025).\n";
                                            } else {
                                                break;
                                            }
                                        }

                                        cout << "Enter Price: ";
                                        cin >> price;

                                        while (true) {
                                            cout << "Select Book Type:\n";
                                            cout << "1. PaperBook\n";
                                            cout << "2. EBook\n";
                                            cout << "3. Showcase Book\n";
                                            cout << "Enter choice: ";
                                            bookType = getValidIntInput();

                                            if (bookType == 1) {
                                                cout << "Enter Stock: ";
                                                stock = getValidIntInput();
                                                store.addBook(new PaperBook(ISBN, title, year, price, author, stock));
                                                cout << "[Quantum Book Store] PaperBook added successfully.\n";
                                                break;
                                            } else if (bookType == 2) {
                                                while (true) {
                                                    cout << "Select File Type:\n";
                                                    cout << "1. PDF (.pdf)\n";
                                                    cout << "2. EPUB (.epub)\n";
                                                    cout << "3. DOCX (.docx)\n";
                                                    cout << "Enter choice: ";
                                                    int typeChoice = getValidIntInput();

                                                    if (typeChoice == 1) {
                                                        fileType = ".pdf";
                                                        break;
                                                    } else if (typeChoice == 2) {
                                                        fileType = ".epub";
                                                        break;
                                                    } else if (typeChoice == 3) {
                                                        fileType = ".docx";
                                                        break;
                                                    } else {
                                                        cout << "[Quantum Book Store] Invalid file type selected. Please choose from 1 to 3.\n";
                                                    }
                                                }
                                                store.addBook(new EBook(ISBN, title, year, price, author, fileType));
                                                cout << "[Quantum Book Store] EBook added successfully.\n";
                                                break;
                                            } else if (bookType == 3) {
                                                store.addBook(new ShowcaseBook(ISBN, title, year, price, author));
                                                cout << "[Quantum Book Store] Showcase Book added successfully.\n";
                                                break;
                                            } else {
                                                cout << "[Quantum Book Store] Invalid book type selected.\n";
                                            }
                                        }
                                        break;
                                    }

                                    case 2: {
                                        int maxAge;
                                        cout << "Enter maximum age in years to remove outdated books: ";
                                        maxAge = getValidIntInput();
                                        store.removeOutdatedBooks(maxAge);
                                        cout << "[Quantum Book Store] Outdated books removed.\n";
                                        break;
                                    }

                                    case 3:
                                        cout << "\n[Quantum Book Store] Current Inventory:\n";
                                        store.displayInventory();
                                        break;

                                    default:
                                        cout << "[Quantum Book Store] Invalid option in Admin Panel. Please try again.\n";
                                }
                            } while (adminChoice != 0);
                            break;
                        }

                        case 2: {
                            int customerChoice;
                            do {
                                cout << "\n--- Customer View ---\n";
                                cout << "1. View All Books\n";
                                cout << "2. Buy a Book\n";
                                cout << "0. Back to Main Menu\n";
                                cout << "Enter your choice: ";
                                customerChoice = getValidIntInput();

                                switch (customerChoice) {
                                    case 0:
                                        break;

                                    case 1:
                                        cout << "\n[Quantum Book Store] Available Books:\n";
                                        store.displayInventory();
                                        break;

                                    case 2: {
                                        int bookType;
                                        string ISBN, email, address;
                                        int quantity = 1;

                                        while (true) {
                                            cout << "What type of book are you trying to buy?\n";
                                            cout << "1. PaperBook\n";
                                            cout << "2. EBook\n";
                                            cout << "Enter choice: ";
                                            bookType = getValidIntInput();

                                            if (bookType == 1 || bookType == 2) break;
                                            else {
                                                cout << "[Quantum Book Store] Invalid book type selected. Please try again.\n";
                                            }
                                        }

                                        cout << "Enter ISBN of the book: ";
                                        cin >> ISBN;

                                        if (bookType == 1) {
                                            cout << "Enter Quantity: ";
                                            quantity = getValidIntInput();
                                            cout << "Enter Address: ";
                                            cin >> address;
                                        } else if (bookType == 2) {
                                            cout << "Enter Email: ";
                                            cin >> email;
                                        }

                                        try {
                                            double total = store.buyBook(ISBN, quantity, email, address);
                                            cout << "[Quantum Book Store] Purchase successful!\n";
                                            cout << "[Quantum Book Store] Total amount paid: $" << total << endl;
                                        } catch (exception& e) {
                                            cout << e.what() << endl;
                                        }
                                        break;
                                    }

                                    default:
                                        cout << "[Quantum Book Store] Invalid customer option. Please try again.\n";
                                }
                            } while (customerChoice != 0);
                            break;
                        }

                        default:
                            cout << "[Quantum Book Store] Invalid choice. Please try again.\n";
                    }
                } while (mainChoice != 0);
                break;
            }

            default:
                cout << "[Quantum Book Store] Invalid mode choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
