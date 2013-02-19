from consumers import RootConsumer


if __name__ == "__main__":
    results = None
    with open('syntax_test.mogu') as f:
        input = f.read()
        results = RootConsumer.parse(input)
