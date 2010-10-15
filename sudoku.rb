def read_matrix
  matrix = []
 
  (0..8).each { |i|
    l = readline
    matrix[i] = []
    (0..8).each { |j|
      matrix[i][j] = l[j..j].to_i
    }
  }
  matrix
end
 
def permissible(matrix, i, j)
  ok = [true,true,true,true,true,true,true,true,true]
  # Same as another in the column isn't permissible...
  (0..8).each { |i2|
    next if matrix[i2][j] == 0
    ok[matrix[i2][j] - 1] = false
  }
  # Same as another in the row isn't permissible...
  (0..8).each { |j2|
    next if matrix[i][j2] == 0
    ok[matrix[i][j2] - 1] = false
  }
  # Same as another in the 3x3 block isn't permissible...
  igroup = (i / 3) * 3
  jgroup = (j / 3) * 3
  (igroup..(igroup + 2)).each { |i2|
    (jgroup..(jgroup + 2)).each { |j2|
      next if matrix[i2][j2] == 0
      ok[matrix[i2][j2] - 1] = false
    }
  }
  # Convert to the array format...
  ret = []
  (0..8).each { |i2| ret.push(i2 + 1) if ok[i2] }
  ret
end
 
def deep_copy_sudoku(matrix)
  newmat = []
  (0..8).each { |i|
    newmat[i] = []
    (0..8).each { |j|
      newmat[i][j] = matrix[i][j]
    }
  }
  newmat
end
 
def solve_sudoku(matrix)
  while true
    options = []
    (0..8).each { |i|
      (0..8).each { |j|
        next if matrix[i][j] != 0
        p = permissible(matrix, i, j)
        # If nothing is permissible, there is no solution at this level.
        return false if (p.length == 0)
        options.push({:i => i, :j => j, :permissible => p})
      }
    }
    # If the matrix is complete, we have a solution...
    return matrix if options.length == 0
 
    omin = options.min { | a, b |
      a[:permissible].length <=> b[:permissible].length
    }
 
    # If there is an option with only one solution, set it and re-check permissibility
    if omin[:permissible].length == 1
      matrix[omin[:i]][omin[:j]] = omin[:permissible][0]
      next
    end
 
    # We have two or more choices. We need to search both...
    omin[:permissible].each { |v|
      mtmp = deep_copy_sudoku(matrix)
      mtmp[omin[:i]][omin[:j]] = v
      ret = solve_sudoku(mtmp)
      if ret != false
        return ret
      end
    }
 
    # We did an exhaustive search on this branch and nothing worked out.
    return false
  end
end
 
def print_matrix(matrix)
  if (matrix == false)
    print "Impossible\n"
    return
  end
 
  (0..8).each { |i|
    (0..8).each { |j|
      print matrix[i][j]
    }
    print "\n"
  }
end
 
print_matrix(solve_sudoku(read_matrix()))
