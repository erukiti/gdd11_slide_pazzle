#! /usr/bin/env ruby

solver = "./solver"

is_only = true
stat_giveup = 0
stat_error = 0
stat_answer = 0
stat_failed = 0
answer_pool = File.open("answer_pool", "a")

resume_list = IO.read('resume2')
resume_list.each_line do |resume|
   problem, shrinked, answer_prefix = resume.split(' ')

  t = Time.now
  result = `#{solver} #{shrinked} #{answer_prefix}`
  t = Time.now - t
  min = (t / 60).to_i
  sec = (t % 60).to_i
  answer = nil

  resume_list = []
  result.each_line do |line|
    case line[0..1]
    when "G:"
      puts "#{problem}: #{min}m #{sec}s #{line}"
      answer = nil
      stat_giveup += 1
    when "A:"
      puts "#{problem}: #{min}m #{sec}s #{line}"
      answer = line[2..-1].strip
      stat_answer += 1
    when "E:"
      puts "#{problem}: #{min}m #{sec}s #{line}"
      answer = nil
      stat_error += 1
    when "R:"
      puts "#{problem}: #{line}" if resume_list.size == 0
      resume_list << line[2..line.size - 2].strip
    end
  end

  unless answer
    answer_file << "\n" unless is_only
    resume_list.each do |resume|
      resume_file << "#{problem},#{resume}\n"
    end
    resume_file.fsync
  else
    # 正否検証プログラム
    width = problem[0 .. 0].to_i
    height = problem[2 .. 2].to_i
    board = problem[4 ... 4 + width * height]
    wall = Array.new(width * height)
    index = 0
    x = nil
    y = nil
    board.each_byte do |chr|
       if chr.chr == "0"
        x = index % width
        y = index / width
      elsif chr.chr == "="
        wall[index] = true
      end

      index += 1
    end

    answer = nil if x == nil || y == nil

    final_board = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[0 ... width * height - 1] + "0"
    index = 0
    wall.each do |flag|
      final_board[index] = "=" if flag
      index += 1
    end

    n_cnt = 0
    index = x + y * width
    answer.each_byte do |hand|
      case hand.chr
      when 'L'
        lx -= 1
        foward = index - 1
        if index % width == 0
          puts "failed: #{n_cnt} out L"
          break
        end
      when 'R'
        rx -= 1
        foward = index + 1
        if foward % width == 0
          puts "failed: #{n_cnt} out R"
          break
        end
      when 'U'
        ux -= 1
        foward = index - width
        if foward < 0
          puts "failed: #{n_cnt} out U"
          break
        end
      when 'D'
        dx -= 1
        foward = index + width
        if foward >= width * height
          puts "failed: #{n_cnt} out D"
          break
        end
      else
        puts "failed: #{n_cnt} unknown hand. #{chr}"
        break
      end
      if wall[index]
        puts "failed: #{n_cnt} WALL!"
        break
      end
      board[index] = board[foward]
      board[foward] = '0'[0]
      index = foward
      n_cnt += 1
    end

    if board == final_board
      answer_file << "#{answer}\n" unless is_only
      answer_pool << "#{problem},#{answer}\n"
      answer_pool.fsync
    else 
      answer_file << "\n" unless is_only
      puts "failed: [#{board}] != [#{final_board}]"
      stat_failed += 1
    end
    answer_file.fsync unless is_only
  end
end

puts "answered: #{stat_answer}, giveuped: #{stat_giveup}, errored: #{stat_error}, failed: #{stat_failed}"
puts "score: #{0.01 * (stat_answer - stat_failed)}"
puts "LX: #{lx}, RX: #{rx}, UX: #{ux}, DX: #{dx}"


