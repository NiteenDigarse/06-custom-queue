#include "custom_queue.hpp"

#include <iostream>
#include <string>
#include <utility>

int main()
{
    std::cout << "===== BASIC TEST =====\n";

    custom::CustomQueue<int> queue(4);

    std::cout << "Empty: " << queue.empty() << '\n';
    std::cout << "Size: " << queue.size() << '\n';

    queue.enqueue(10);
    queue.enqueue(20);
    queue.enqueue(30);

    std::cout << "Front: " << queue.front() << '\n';
    std::cout << "Back: " << queue.back() << '\n';
    std::cout << "Size: " << queue.size() << '\n';


    std::cout << "\n===== FIFO TEST =====\n";

    std::cout << "Dequeuing: ";

    while (!queue.empty()) {
        std::cout << queue.front() << ' ';
        queue.dequeue();
    }

    std::cout << '\n';


    std::cout << "\n===== WRAP-AROUND TEST =====\n";

    queue.enqueue(10);
    queue.enqueue(20);
    queue.enqueue(30);

    queue.dequeue();

    queue.enqueue(40);
    queue.enqueue(50);

    std::cout << "Front: " << queue.front() << '\n';
    std::cout << "Back: " << queue.back() << '\n';
    std::cout << "Size: " << queue.size() << '\n';

    std::cout << "Queue order: ";

    while (!queue.empty()) {
        std::cout << queue.front() << ' ';
        queue.dequeue();
    }

    std::cout << '\n';


    std::cout << "\n===== FULL QUEUE TEST =====\n";

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);

    try {
        queue.enqueue(5);
    }
    catch (const std::runtime_error& e) {
        std::cout << "Exception: " << e.what() << '\n';
    }


    std::cout << "\n===== EMPTY QUEUE TEST =====\n";

    while (!queue.empty()) {
        queue.dequeue();
    }

    try {
        queue.dequeue();
    }
    catch (const std::runtime_error& e) {
        std::cout << "Exception: " << e.what() << '\n';
    }

    try {
        queue.front();
    }
    catch (const std::runtime_error& e) {
        std::cout << "Exception: " << e.what() << '\n';
    }

    try {
        queue.back();
    }
    catch (const std::runtime_error& e) {
        std::cout << "Exception: " << e.what() << '\n';
    }


    std::cout << "\n===== RVALUE TEST =====\n";

    custom::CustomQueue<std::string> names(3);

    std::string name = "Niteen";

    names.enqueue(name);
    names.enqueue(std::string("C++"));
    names.enqueue("Engineering");

    std::cout << names.front() << '\n';
    std::cout << names.back() << '\n';


    std::cout << "\n===== COPY TEST =====\n";

    custom::CustomQueue<int> original(4);

    original.enqueue(100);
    original.enqueue(200);
    original.enqueue(300);

    custom::CustomQueue<int> copied(original);

    std::cout << "Original front: " << original.front() << '\n';
    std::cout << "Copied front: " << copied.front() << '\n';

    original.dequeue();

    std::cout << "Original after dequeue: "
              << original.front() << '\n';

    std::cout << "Copied remains: "
              << copied.front() << '\n';


    std::cout << "\n===== MOVE TEST =====\n";

    custom::CustomQueue<int> source(4);

    source.enqueue(500);
    source.enqueue(600);

    custom::CustomQueue<int> moved(std::move(source));

    std::cout << "Moved front: "
              << moved.front() << '\n';

    std::cout << "Moved size: "
              << moved.size() << '\n';


    std::cout << "\n===== CONST TEST =====\n";

    const custom::CustomQueue<int>& const_queue = moved;

    std::cout << "Const front: "
              << const_queue.front() << '\n';

    std::cout << "Const back: "
              << const_queue.back() << '\n';

    std::cout << "Const size: "
              << const_queue.size() << '\n';

    std::cout << "Const empty: "
              << const_queue.empty() << '\n';


    std::cout << "\n===== ALL TESTS COMPLETED =====\n";

    return 0;
}