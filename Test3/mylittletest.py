import testpkg

counter = 0

def hello():
    print("Hello from python\n")
    return 12

def helloc():
    testpkg.test("Hello from python but through C\n")
    return 42

def count():
    global counter
    counter += 1
    testpkg.test(f"Counter: {counter} (printed through C)\n")
    return counter

if __name__ == "__main__":
    helloc()