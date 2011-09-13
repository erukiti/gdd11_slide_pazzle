#! /usr/bin/env ruby

#3,3,471620538
if ARGV.size != 1
  puts "E: invalid ARGV"
  exit
end

width = ARGV[0][0 .. 0].to_i
height = ARGV[0][2 .. 2].to_i

if width * height > 12
  puts "G: giveup"
  exit
end



if ARGV[0].size != 4 + width * height
  puts "E: invalid data"
  exit
end

initial_board = ARGV[0][4 ... 4 + width * height]

if initial_board.size != width * height
  puts "E: invalid data"
  exit
end

maxnum = width * height

hash_seed = Array.new(width * height * maxnum)
(0 ... width * height * maxnum).each do |index|
  hash_seed[index] = rand(0xffffffffffffffff)
end

wall = Array.new(width * height)
index = 0
initial_board.each_byte do |chr|
  if chr == "0"[0]
    x = index % width
    y = index / width
    puts "D: first space: #{x}, #{y}"
  elsif chr == "="[0]
    wall[index] = true
  end

  index += 1
end

final_board = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[0 ... width * height - 1] + "0"
index = 0
wall.each do |flag|
  final_board[index] = "=" if flag
  index += 1
end
puts "D: #{final_board}"

def chr_to_num(chr)
  if chr >= "0"[0] && chr <= "9"[0]
    chr - "0"[0]
  elsif chr >= "A"[0] && chr <= "Z"[0]
    chr - "A"[0] + 10
  end
end

def hash_from_board(board, hash_seed, maxnum)
  hash = 0
  index = 0
  board.each_byte do |chr|
    hash ^= hash_seed[index * maxnum + chr_to_num(chr)] if chr != '='[0]
    index += 1
  end
  hash
end

final_hash = hash_from_board(final_board, hash_seed, maxnum)

$board_hash = {}
$board_hash[hash_from_board(initial_board, hash_seed, maxnum)] = [0, 0] # C のソース化した時には、もっと別のマジックナンバー入れておく
$board_hash[final_hash] = [0, 0]

#ゴール前のboard_hashを作成

def move_hash(board, hash, hash_seed, index, foward, maxnum)
  hash ^= hash_seed[index * maxnum + 0]
  hash ^= hash_seed[foward * maxnum + chr_to_num(board[foward])]
  hash ^= hash_seed[foward * maxnum + 0]
  hash ^= hash_seed[index * maxnum + chr_to_num(board[foward])]
end

def move(board, depth, hash, hash_seed, index, foward, width, height, maxnum, nextstack)
  hash = move_hash(board, hash, hash_seed, index, foward, maxnum)
  board[index] = board[foward]
  board[foward] = "0"[0]

  if $board_hash[hash]
    if $board_hash[hash] == [0, 0]
      puts "D: solved: "
      answer = ""

      tmp = index
      index = foward
      foward = tmp

      while depth > 0
        puts "D: #{index} => #{foward}"
        if index + 1 == foward
          answer << "R"
        elsif index - 1 == foward
          answer << "L"
        elsif index + width == foward
          answer << "D"
        elsif index - width == foward
          answer << "U"
        else
          puts "E: solving BUGS"
          exit
        end
        hash = move_hash(board, hash, hash_seed, index, foward, maxnum)
        board[index] = board[foward]
        board[foward] = "0"[0]

        index = foward
        dummy, foward = $board_hash[hash]

        depth -= 1
      end

      puts "A: #{answer}"

      exit
    end
    return if -$board_hash[hash][0] <= depth
  end
  $board_hash[hash] = [-depth, index]
  nextstack << [foward, index, hash, board] if nextstack
end

def create_finish_hash(final_board, final_hash, wall, hash_seed, width, height, maxnum)
  prev = width * height
  index = width * height - 1

  stack = [[index, prev, final_hash, final_board]]
  depth = 1
  while stack
    nextstack = []
    puts "D: depth: #{depth}, #{stack.size}"
    stack.each do |index, prev, hash, board|
      if index - width != prev && index - width >= 0 && !wall[index - width]
        move(board.dup, depth, hash, hash_seed, index, index - width, width, height, maxnum, nextstack)
      end
      if index + width != prev && index + width < width * height && !wall[index + width]
        move(board.dup, depth, hash, hash_seed, index, index + width, width, height, maxnum, nextstack)
      end
      if index - 1 != prev && index % width > 0  && !wall[index - 1]
        move(board.dup, depth, hash, hash_seed, index, index - 1, width, height, maxnum, nextstack)
      end
      if index + 1 != prev && (index + 1) % width > 0 && !wall[index + 1]
        move(board.dup, depth, hash, hash_seed, index, index + 1, width, height, maxnum, nextstack)
      end
    end
    stack = nextstack

    break if stack.size > 1000 * 1000
    depth += 1
  end
  stack = nil
  nextstack = nil
  GC.start
end

create_finish_hash(final_board, final_hash, wall, hash_seed, width, height, maxnum)
puts "D:#{$board_hash.size}"
puts "G: give up"
