import random

G = 6.67e-11

class Vector:
    def __init__(self, components):
        self.components = components[:]
        self.size = len(components)

    def __add__(self, other):
        """ Returns vector addition """
        if self.size != other.size:
            raise AssertionError("Addition attempted with vectors of different sizes")
        return Vector([self.components[i] + other.components[i] 
                        for i in range(self.size)])

    def  __sub__(self, other):
        """ Returns vector subtraction """
        if self.size != other.size:
            raise AssertionError("Subtraction attempted with vectors of different sizes")
        return Vector([self.components[i] - other.components[i]
                        for  i in range(self.size)])

    def __mul__(self, other):
        """ Returns point product """
        if self.size != other.size:
            raise AssertionError("Point product attempted with vectors of different sizes")
        return sum(self.components[i] * other.components[i]
                    for i in range(self.size))

    def __eq__(self, other):
        """ Compares components between vectors """
        return self.components == other.components
    
    def __ne__(self, other):
        """ Compares components between vectors """
        return self.components != other.components

    def get_magnitude(self):
        """ Returns norm or magnitude """
        return sum([component ** 2 for component in self.components])**0.5
        
    def pow(self, exponent):
        for i in range(self.size):
            self.components[i] = self.components[i]**exponent
        return self

    def scale(self, scalar):
        for i in range(self.size):
            self.components[i] = scalar * self.components[i]
        return self

    def __str__(self):
        return str(self.components)

class Body:
    """ Perfectly spherical body """
    def __init__(self, mass, radius, position, velocity):
        self.mass = mass
        self.radius = radius
        self.position = position
        self.velocity = velocity
        self.acceleration = Vector([0,0,0])

    def add_radiuses(self, other_body):
        return (self.radius ** 3 + other_body.radius ** 3)**(0.33)

    def update_acceleration(self, other_body):
        distance = other_body.position - self.position
        magnitude = distance.get_magnitude()
        distance.scale(other_body.mass / (magnitude**3))
        self.acceleration = self.acceleration + distance

    def update_velocity(self, delta_t):
        self.velocity = self.velocity + self.acceleration.scale(delta_t)

    def update_position(self, delta_t):
        self.position = self.position + self.velocity.scale(delta_t)

    def check_collision(self, other_body):
        distance = self.position - other_body.position
        if distance.get_magnitude() < (self.radius + other_body.radius):
            return True
        return False

    def deactivate(self):
        self.mass *= -1

    def merge_velocities(self, other):
        self.velocity = self.velocity.scale(self.mass) + other.velocity.scale(other.mass)
        self.velocity.scale(1/(self.mass + other.mass))

    def absorb(self, other):
        if self.mass >= other.mass:
            self.mass += other.mass
            other.deactivate()
            self.add_radiuses(other)
            self.merge_velocities(other)

    def __eq__(self, other):
        """ Compares components between vectors """
        return self.mass == other.mass and \
            self.radius == other.radius and \
            self.position == other.position

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return f"Mass: {self.mass}\nRadius: {self.radius}\nPosition: {self.position}\nVelocity: {self.velocity}\nAcceleration: {self.acceleration}"
            

class Simulation:
    def __init__(self, universe_file, delta_t, max_simul_time):
        self.universe_file = universe_file
        self.delta_t = delta_t
        self.max_simul_time = max_simul_time
        self.bodies = []
        self.body_count = 0
        self.active_bodies = 0

    def create_universe(self):
        with open(self.universe_file, "r") as file:
            try:
                self.body_count = int(file.readline())
                header = file.readline()
                for i in range (self.body_count):
                    body_data = file.readline().split("\t")
                    mass = float(body_data[0])
                    radius = float(body_data[1])
                    position = Vector([float(body_data[2]),
                                       float(body_data[3]),
                                       float(body_data[4])])
                    initial_velocity = Vector([float(body_data[5]),
                                               float(body_data[6]),
                                               float(body_data[7])])
                    self.bodies.append(Body(mass,
                                            radius,
                                            position,
                                            initial_velocity))
            except RuntimeError:
                print("Body count invalid")
        self.active_bodies = self.body_count

    def run(self):
        self.create_universe()
        simulated_time = 0.0
        while simulated_time < self.max_simul_time and self.active_bodies > 1:
            self.check_collisions()
            self.update_velocities()
            self.update_positions()
            simulated_time = simulated_time + self.delta_t
        self.report_results()
    
    def check_collisions(self):
        for body in self.bodies:
            for other in self.bodies:
                if body != other and body.check_collision(other) \
                    and body.mass > 0 and other.mass > 0:
                    self.active_bodies -= 1
                    if body.mass <= other.mass:
                        other.absorb(body)
                    else:
                        body.absorb(other)

    def update_accelerations(self):
        # Evaluate each body with the other bodies
        for i in range(len(self.bodies)):
            body = self.bodies[i]
            body.acceleration.scale(0)
            # summation of accelerations with the other bodies
            for j in range(len(self.bodies)):
                if i == j:
                    continue
                body.update_acceleration(self.bodies[j])
            body.acceleration.scale(-G)

    def update_velocities(self):
        self.update_accelerations()
        for body in self.bodies:
            body.update_velocity(self.delta_t)

    def update_positions(self):
        for body in self.bodies:
            body.update_position(self.delta_t)

    def distance_mean(self):
        total_distance = Vector([0, 0, 0])
        count = 0
        for i in range(len(self.bodies)-1):
            for j in range(i + 1, len(self.bodies)):
                if self.bodies[i].mass > 0 and self.bodies[j].mass > 0:
                    distance = self.bodies[i].position - self.bodies[j].position
                    total_distance += distance
                    count += 1
        return total_distance.scale(1/count) if count > 0 else 0.0

    def distance_stdev(self):
        mean_distance = self.distance_mean()
        total_distance = Vector([0, 0, 0])
        count = 0
        for i in range(len(self.bodies)-1):
            for j in range(i + 1, len(self.bodies)):
                if self.bodies[i].mass > 0 and self.bodies[j].mass > 0:
                    distance = self.bodies[i].position - self.bodies[j].position
                    total_distance += (distance - mean_distance).pow(2)
                    count += 1
        return (total_distance.scale(1/count)).pow(0.5) if count > 0 else 0.0

    def velocity_mean(self):
        total_velocity = Vector([0, 0, 0])
        count = 0
        for body in self.bodies:
            if body.mass > 0:
                total_velocity += body.velocity
                count += 1
        return total_velocity.scale(1 / count) if count > 0 else 0.0
    
    def velocity_stdev(self):
        mean_velocity = self.velocity_mean()
        total_squared_distance = Vector([0, 0, 0])
        count = 0
        for body in self.bodies:
            if body.mass > 0:
                diff = body.velocity - mean_velocity
                total_squared_distance += diff.pow(2)
                count += 1
        if count == 0:
            return 0
        return (total_squared_distance.scale(1/count)).pow(0.5)

    def remaining(self):
        return sum([1 for body in self.bodies if body.mass > 0])

    def report_results(self):
        print(f"Remaining bodies: {self.remaining()}\n"
              f"Distance (mean): {self.distance_mean()}\n"
              f"Distance (stdev): {self.distance_stdev()}\n"
              f"Velocity (mean): {self.velocity_mean()}\n"
              f"Velocity (stdev): {self.velocity_stdev()}\n")

def main():
    simulation = Simulation("../universes/univ001.tsv", 1, 10)
    simulation.run()

if __name__ == "__main__":
    main()
