import time
import math

class House:
    def __init__(self, file_path):
        self.load_parameters(file_path)
        self.create_error_name(file_path)
        self.current_battery_steps = self.max_battery
        self.current_location = self.docking_station_location

    def load_parameters(self, file_path):
        # Assuming some function 'get_house_parameters' reads from the file and populates the variables
        self.house_name, self.max_steps, self.max_battery, self.amount_of_dirt, self.rows, self.cols, self.docking_station_location, self.house_surface = get_house_parameters(file_path)

    def create_error_name(self, house_file_path):
        house_name = house_file_path.split('/')[-1].split('.')[0]
        self.error_file_name = house_name + ".error"

    def get_house_surface(self):
        return self.house_surface

    def get_max_steps(self):
        return self.max_steps

    def get_max_battery(self):
        return self.max_battery

    def get_amount_of_dirt(self):
        return self.amount_of_dirt

    def get_docking_station_location(self):
        return self.docking_station_location

    def is_wall(self, location):
        row, col = location
        if row < 0 or row >= self.rows or col < 0 or col >= self.cols:
            return True
        return self.house_surface[row][col] == Elements.WALL

    def update_location(self, step):
        step_elements = Step.STEP_MAP[step]
        self.current_location = (self.current_location[0] + step_elements[0],
                                 self.current_location[1] + step_elements[1])

    def make_step(self, step):
        if step == Step.STAY:
            if self.current_location == self.docking_station_location:
                self.charge()
            else:
                self.clean()
                self.discharge()
        else:
            if self.is_wall(step):
                raise ValueError("Error: step is toward the wall!")
            self.discharge()
        self.update_location(step)

    def charge(self):
        self.current_battery_steps = min(self.current_battery_steps + self.max_battery / 20.0, self.max_battery)

    def discharge(self):
        self.current_battery_steps -= 1

    def clean(self):
        self.amount_of_dirt -= 1
        row, col = self.current_location
        self.house_surface[row][col] -= 1


class VisualSimulation:
    def __init__(self):
        self.steps = []
        self.map = []
        self.first_run = True
        self.direction_string_map = {
            Step.NORTH: "^",
            Step.EAST: ">",
            Step.SOUTH: "v",
            Step.WEST: "<",
            Step.STAY: "-",
            Step.FINISH: "F"
        }

    def process_files(self, input_file_name, output_file_name):
        self.process_input_file(input_file_name)
        self.process_output_file(output_file_name)

    def process_input_file(self, file_name):
        h = House(file_name)
        self.load_parameters(h)
        self.load_house_mapping(h)

    def process_output_file(self, file_name):
        with open(file_name, 'r') as file:
            lines = file.readlines()
            self.num_steps = int(lines[0].split('=')[1].strip())
            self.dirt_left = int(lines[1].split('=')[1].strip())
            self.status = lines[2].split('=')[1].strip()  # Keep status as a string
            self.indock = lines[3].split('=')[1].strip().upper() == "TRUE"  # Convert to boolean
            self.score = int(lines[4].split('=')[1].strip())

            if self.num_steps == 0:
                return

            steps_line = lines[6].strip()
            self.steps = [self.char_to_step(c) for c in steps_line]

    def char_to_step(self, c):
        return {
            'N': Step.NORTH,
            'E': Step.EAST,
            'S': Step.SOUTH,
            'W': Step.WEST,
            's': Step.STAY,
            'F': Step.FINISH,
        }[c]

    def load_parameters(self, house):
        self.max_battery_steps = house.get_max_battery()
        self.battery_steps = self.max_battery_steps
        self.allowed_number_of_steps = house.get_max_steps()
        self.amount_of_dirt = house.get_amount_of_dirt()
        self.docking_station_location = house.get_docking_station_location()
        self.robot_location = self.docking_station_location

    def int_to_house_element_string(self, num):
        if num == Elements.DOCKING_STATION:
            return 'D'
        elif num == Elements.WALL:
            return '#'
        else:
            return str(num)

    def load_house_mapping(self, house):
        house_surface = house.get_house_surface()
        for row in house_surface:
            self.map.append([self.int_to_house_element_string(num) for num in row])

    def print_house(self, print_dir=False):
        if not self.first_run:
            self.clear_map_area(len(self.map) + 3)

        for i, row in enumerate(self.map):
            for j, cell in enumerate(row):
                if (i, j) == self.robot_location:
                    if print_dir:
                        print(f" {self.direction_string_map[self.current_step]} ", end="")
                    else:
                        print(f" R ", end="")
                else:
                    print(f" {cell} ", end="")
            print()

        print(f"Left steps in battery: {math.floor(self.battery_steps)}, out of {self.max_battery_steps}")
        print(f"Allowed number of steps: {self.allowed_number_of_steps}")

    def clear_map_area(self, rows):
        print("\033[H\033[J", end="")

    def update_map(self):
        step_elements = Step.STEP_MAP[self.current_step]
        self.robot_location = (self.robot_location[0] + step_elements[0],
                               self.robot_location[1] + step_elements[1])

        if self.current_step == Step.STAY:
            if self.robot_location == self.docking_station_location:
                self.charge()
            else:
                self.battery_steps -= 1
                self.clean()
        else:
            self.battery_steps -= 1
        self.allowed_number_of_steps -= 1

    def start_simulation(self):
        self.first_run = True
        print("SIMULATION STARTS ...")

        self.print_house(False)
        self.first_run = False
        time.sleep(1)
        
        for step in self.steps:
            self.current_step = step
            if self.current_step == Step.FINISH:
                break

            self.print_house(True)
            time.sleep(1)

            self.update_map()
            self.print_house(False)
            time.sleep(1)

        print("SIMULATION ENDED")
        time.sleep(1)

    def charge(self):
        self.battery_steps += self.max_battery_steps / 20
        if self.battery_steps > self.max_battery_steps:
            self.battery_steps = self.max_battery_steps

    def clean(self):
        dirt = self.map[self.robot_location[0]][self.robot_location[1]]
        # Assuming there's a way to determine the level of dirt and clean accordingly
        self.map[self.robot_location[0]][self.robot_location[1]] = max(0, int(dirt) - 1)


class Step:
    NORTH = 'N'
    EAST = 'E'
    SOUTH = 'S'
    WEST = 'W'
    STAY = 's'
    FINISH = 'F'

    STEP_MAP = {
        NORTH: (-1, 0),
        EAST: (0, 1),
        SOUTH: (1, 0),
        WEST: (0, -1),
        STAY: (0, 0),
    }


class Elements:
    DOCKING_STATION = 1
    WALL = 2
    ROBOT = 3
    DIRECTION = 4


def main():
    input_file_name = "example_1.house"
    output_file_name = "example_1-Algo_209228600_A.txt"
    
    simulation = VisualSimulation()
    
    # Process the input and output files
    simulation.process_files(input_file_name, output_file_name)
    
    # Start the simulation
    simulation.start_simulation()

if __name__ == "__main__":
    main()