# Values

### Booleans


### Numbers


### Strings

### Null

# Array


# Map

# Enums

We’ll first start off with numeric enums, which are probably more familiar if you’re coming from other languages. An enum can be defined using the enum keyword.

```
enum Direction {
  Up = 1,
  Down,
  Left,
  Right,
}

```

If we wanted, we could leave off the initializers entirely:

```
enum Direction {
  Up,
  Down,
  Left,
  Right,
}

```

Using an enum is simple: just access any member as a property off of the enum itself, and declare types using the name of the enum:

```
enum UserResponse {
  No = 0,
  Yes = 1,
}
 
function respond(recipient: string, message: UserResponse): void {
  // ...
}
 
respond("Princess Caroline", UserResponse.Yes);
```

String enums are a similar concept, but have some subtle runtime differences as documented below. In a string enum, each member has to be constant-initialized with a string literal, or with another string enum member.

```
enum Direction {
  Up = "UP",
  Down = "DOWN",
  Left = "LEFT",
  Right = "RIGHT",
}

```


# Function

The simplest way to describe a function is with a function type expression. These types are syntactically similar to arrow functions:

```
function greeter(fn: (a: string) => void) {
  fn("Hello, World");
}
 
function printToConsole(s: string) {
  console.log(s);
}
 
greeter(printToConsole);

```
Of course, we can use a type alias to name a function type:

```
type GreetFunction = (a: string) => void;
function greeter(fn: GreetFunction) {
  // ...
}
```

We can model this in TypeScript by marking the parameter as optional with ?:

```
function f(x?: number) {
  // ...
}
f(); // OK
f(10); // OK

```

You can also provide a parameter default:

```
function f(x = 10) {
  // ...
}

f(); // OK
f(10); // OK

```

# Class

A field declaration creates a public writeable property on a class:

```
class Point {
  x: number;
  y: number;
}
 
const pt = new Point();
pt.x = 0;
pt.y = 0;

```

Fields can also have initializers; these will run automatically when the class is instantiated:

```
class Point {
  x = 0;
  y = 0;
}
 
const pt = new Point();

```

Classes may extend from a base class. A derived class has all the properties and methods of its base class, and can also define additional members.

```
class Animal {
  move() {
    console.log("Moving along!");
  }
}
 
class Dog extends Animal {
  woof(times: number) {
    for (let i = 0; i < times; i++) {
      console.log("woof!");
    }
  }
}
 
const d = new Dog();
// Base class method
d.move();
// Derived class method
d.woof(3);

```

A derived class can also override a base class field or property. You can use the super. syntax to access base class methods

```
class Base {
  greet() {
    console.log("Hello, world!");
  }
}
 
class Derived extends Base {
  greet(name?: string) {
    if (name === undefined) {
      super.greet();
    } else {
      console.log(`Hello, ${name.toUpperCase()}`);
    }
  }
}
 
const d = new Derived();
d.greet();
d.greet("reader");

```

