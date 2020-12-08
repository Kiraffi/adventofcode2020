
use std::{collections::HashSet, fs::File};
use std::io::{self, BufRead};
use std::path::Path;

struct Command 
{
	name: String,
	value: i32
}

fn main() 
{
	let mut program: Vec<Command> = Vec::new();

	// File hosts must exist in current path before this produces output
	if let Ok(lines) = read_lines("src/data.txt")
	{
		for line in lines 
		{
			if let Ok(ip) = line 
			{
				let words: Vec<&str> = ip.split_whitespace().collect();
   
				let v: i32 = words[1].parse().unwrap();

				program.push(Command{name: words[0].to_string(), value: v});
			}
		}
	}

	{
		println!("Part a:");
		let (_, accum) = exec_program(&program);
		println!("Accumulated: {}", accum);
	}

	{
		for i in 0..program.len()
		{
			match program[i].name.as_str()
			{
				"nop" => program[i].name = "jmp".to_string(),
				"jmp" => program[i].name = "nop".to_string(),
				_ => {} 
			}
			
			let (result, accum) = exec_program(&program);
			if result == 1
			{
				println!("Finished: Accumulated: {}", accum);
				break;
			}
			match program[i].name.as_str()
			{
				"nop" => program[i].name = "jmp".to_string(),
				"jmp" => program[i].name = "nop".to_string(),
				_ => {} 
			}
		}
	}
}

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, 
{
	let file = File::open(filename)?; 
	Ok(io::BufReader::new(file).lines())
}

fn exec_program(program: &Vec<Command>) -> (i32, i32)
{
	let mut result = 0;

	let mut program_ip: i32 = 0;
	let mut visited_lines: HashSet<i32> = HashSet::new();
	let mut accum: i32 = 0;
	while result == 0 
	{
		if visited_lines.contains(&program_ip)
		{
			result = -1;
			break;
		}
		else if program_ip >= program.len() as i32
		{
			result = 1;
			break;
		}
		visited_lines.insert(program_ip);
		let val: i32 = program[program_ip as usize].value;
		match program[program_ip as usize].name.as_str()
		{
			"nop" => 
			{
				//println!("{} nop", program_ip);
				program_ip += 1;
			},
			"acc" => 
			{ 
				//println!("{} acc: {} ", program_ip, val);
				accum += val;
				program_ip += 1;
			},
			"jmp" =>
			{
				//println!("{} jmp to {}", program_ip, program_ip + val);
				program_ip += val;
			},
			_ =>
			{
				println!("Unknown command: {}", program[program_ip as usize].name);
				panic!("Unknown command!");
			}
		}
	}
	return (result, accum);
}