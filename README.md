# Cobra

Cobra is a programming language for Mobile App cross-platform dynamic scenarios. Its goal is to be faster and lighter than JS

## Hello World!

Be bold! End every statement with an exclamation mark!

```java
print("Hello world")
```


## Declaring a variable


```java
var someNumber = 10;
var someString = "hello";
var someBoolean = true;
var someFalseBoolean = false;
var someNilValue = nil;
var someOtherNilValue; // This variable is equal to nil
var someList = [1, 2];
var someDict = {"key": 0, "key1": 1};

```

### Constants


Constant variables can be edited, but not re-assigned.

```java
const x = 10;
x = 15; // [line 1] Error at '=': Cannot assign to a constant.
print(x); // 10
```

## Strings

### Concatenation
Use a plus sign to join string together. This is called concatenation.


```java
"This item costs " + "27" + " euros"; // "This item costs 27 euros"
```

### Indexing
You can access a specific letter in a string with the same syntax you would use to access an item in a list.

```java
string[0]; // D
string[100]; // String index out of bounds.

for (var i = 0; i < x.len(); i += 1) {
    print(string[i]); 
}
// D
// i
// c
// t
// u
```

### string.length() -> Number
```java
"string".len(); // 6
```

## Collections

### List


### Dictionary


## Control Flow



## Function

### Regular Functions
```java
//Define the function 
def someFunction() {
    print("Hello!");
}

someFunction(); // Call the function

def someFunctionWithArgs(arg1, arg2) {
    print(arg1 + arg2);
}

someFunctionWithArgs(5, 5); // 10
someFunctionWithArgs("test", "test"); // testtest

def someFunctionWithReturn() {
    return 10;
}

print(someFunctionWithReturn()); // 10
```

## Class

### Defining a class

```java
class SomeClass {
    // Constructor
    init() {
        print("Object created!");
    }
}

SomeClass(); // Object created!
```

### Access Levels
```java
class SomeClass {
    private x;
    
    init(x) {
        this.x = x;
    }

    private getX() {
        return this.x;
    }
}

print(SomeClass(10).getX()); // 10
print(SomeClass(10).x); // Cannot access private attribute 'x' on 'SomeClass' instance.
```


## Enum

