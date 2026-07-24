#!/usr/bin/ruby -w

## Generate Catch2 TEST_CASE file by template

require 'rexml/document'


## Get compound representing class or struct from doxygen xml file
def get_compound(class_name, doxygen_xml_dir = 'xml')

  glob_expr = "#{doxygen_xml_dir}/{class,struct}*#{class_name}.xml"
  xml_files = Dir.glob(glob_expr)

  ## locate xml file for class
  compound = xml_files.find do |f|
    doc = REXML::Document.new(File.read(f))
    compound = REXML::XPath.first(doc,
      "//compounddef[substring(compoundname, string-length(compoundname) - string-length('#{class_name}') + 1) = '#{class_name}']")
    return compound
  end

  return nil
end


## Get list of member functions of class/struct
def get_member_functions(compound)

  return [] if compound.nil?

  methods = []

  # extract all functions (memberdef mit kind="function")
  compound.each_element('//memberdef[@kind="function"]') do |member|
    name = member.elements['name']&.text
    methods << name if name
  end

  methods.uniq
end


## Extract list of member functions from class/struct
def extract_member_functions_from_class(class_name)

  compound = get_compound(class_name)
  get_member_functions(compound)
end


## Expand $<SECTIONS>
def expand_template(line, replacements)
  line.gsub(/\$\<(\w+)\>/) do |match|
    key = $1

    if key == "SECTIONS"
      line = ""
      function_list = extract_member_functions_from_class(replacements['CLASS'])
      function_list.each do |f|
        line << "\tSECTION (\"#{f} is correct\")\n"
        line << "\t{\n"
        line << "\t\tFAIL (\"#{f} test is missing\");\n"
        line << "\t}\n"
      end
      return line
    end
  end
end


## Replace any occurrence of ${KEY} in line by MODIFIER
## for all pairs 'KEY=MODIFIER' in replacements.
def replace_template(line, replacements)
  line.gsub(/\$\{(\w+)(?::([lu]))?\}/) do |match|
    key = $1
    modifier = $2

    value = replacements[key.to_sym] || replacements[key] || match
    value = value.to_s

    case modifier
    when 'l'
      value.downcase
    when 'u'
      value.upcase
    else
      value
    end
  end
end


## Apply replace_template() to each line in input_file and write the
## result to output_file.
def process_template_file(input_file, output_file, replacements)
  File.open(output_file, 'w') do |out|
    File.foreach(input_file) do |line|
      processed_line = replace_template(line, replacements)
      processed_line = expand_template(processed_line, replacements)
      out.write(processed_line)
    end
  end
  puts output_file
end

## -----------------------------------------------------------------------------

#puts "ARGV: #{ARGV}"

## Check command line
if ARGV.length < 3
  puts "Usage: ruby create_testcase.rb <template-file> <outfile> <key1=value1> [key2=value2] ..."
  exit 1
end

template_file = ARGV[0]
output_file   = ARGV[1]

## Parse input replacements
replacements = {}
ARGV[2..-1].each do |arg|
  key, value = arg.split('=', 2)
  if key && value
    replacements[key] = value
  else
    puts "Error: argument '#{arg}' must have format 'KEY=VALUE'"
    exit 1
  end
end

process_template_file(template_file, output_file, replacements)

