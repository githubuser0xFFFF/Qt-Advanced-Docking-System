import os

from pyqtbuild import PyQtBindings, PyQtProject
from sipbuild import Option

class PyQtAds(PyQtProject):
	def __init__(self):
		""" Initialise the project. """

		super().__init__()

		self.bindings_factories = [ads]

class ads(PyQtBindings):
	def __init__(self, project):
		""" Initialise the bindings. """

		super().__init__(project, 'ads')

	def apply_user_defaults(self, tool):
		""" Set default values for user options that haven't been set yet. """

		resource_file = os.path.join(self.project.root_dir,'src','ads.qrc')
		print("Adding resource file to qmake project: ", resource_file)
		self.builder_settings.append('RESOURCES += '+resource_file)
		
		super().apply_user_defaults(tool)
